/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * mdev_adc.c: mdev driver for ADC
 */
#include <mc200.h>
#include <mc200_gpio.h>
#include <mc200_pinmux.h>
#include <mdev_adc.h>
#include <mc200_dma.h>
#include <wm_os.h>

#define SEMAPHORE_NAME_LEN 15
#define MC200_ADC_NUM 2
#define MAX_DMA_BLK_SIZE 1023

static os_semaphore_t adc_sem[MC200_ADC_NUM];

/* adc mdev and synchronization handles */
static mdev_t mdev_adc[MC200_ADC_NUM];
static os_mutex_t adc_mutex[MC200_ADC_NUM];

static void adcdrv_dma_irq_handler(int port_id);

static void (*adc_drv_dma_irq_handler[MC200_ADC_NUM])();

static uint32_t adc_tout[MC200_ADC_NUM];

#define DEFINE_ADC_CB(id) \
	static void \
	adcdrv ## id ## _ ## dma_irq_handler() {\
		adcdrv_ ## dma_irq_handler(id); }

#define GET_ADC_CB(id) adcdrv ## id ## _ ## dma_irq_handler

DEFINE_ADC_CB(0)
DEFINE_ADC_CB(1)

DMA_CFG_Type dmaconfig;

/* adc device specific names */
static const char *mdev_adc_name[MC200_ADC_NUM] = {
	"MDEV_ADC0",
	"MDEV_ADC1",
};

typedef struct adc_priv_data {
	ADC_Channel_Type channelx;
}adc_data_t;

static adc_data_t adcdrv_data[MC200_ADC_NUM];

ADC_CFG_Type adcConfig = {.adcResolution = ADC_RESOLUTION_16BIT,
			.adcVrefSource = ADC_VREF_INTERNAL,
			.adcGainSel = ADC_GAIN_1,
			.adcClockDivider =	ADC_CLOCK_DIVIDER_32,
			.adcBiasMode =	ADC_BIAS_FULL
};

static void fill_cb_array()
{
	adc_drv_dma_irq_handler[0] =  GET_ADC_CB(0);

	adc_drv_dma_irq_handler[1] =  GET_ADC_CB(1);
}

void adc_get_config(ADC_CFG_Type *config)
{
	*config = adcConfig;
}

void adc_modify_default_config(adc_cfg_param config_type, int value)
{
	switch (config_type) {
	case adcResolution:
			if ((ADC_RESOLUTION_10BIT <= value) &&
					(value <= ADC_RESOLUTION_16BIT))
				adcConfig.adcResolution = value;
		break;

	case adcVrefSource:
		if ((ADC_VREF_VCAU <= value) &&
				(value <= ADC_VREF_INTERNAL))
			adcConfig.adcVrefSource = value;
		break;

	case adcGainSel:
		if ((ADC_GAIN_0P5 <= value) &&
				(value <= ADC_GAIN_2))
			adcConfig.adcGainSel = value;
		break;

	case adcClockDivider:
		if ((ADC_CLOCK_DIVIDER_4 <= value) &&
				(value <= ADC_CLOCK_DIVIDER_32))
			adcConfig.adcClockDivider = value;
		break;

	case adcBiasMode:
		if ((ADC_BIAS_FULL <= value) &&
				(value <= ADC_BIAS_HALF))
			adcConfig.adcBiasMode = value;
		break;

	default:
		break;
	}
}

mdev_t *adc_drv_open(ADC_ID_Type adc_id, ADC_Channel_Type channel)
{
	adc_data_t *adc_data_p;
	int ret;
	mdev_t *mdev_p = mdev_get_handle(mdev_adc_name[adc_id]);

	if (mdev_p == NULL) {
		ADC_LOG("driver open called before register (%s)\r\n",
			mdev_adc_name[adc_id]);
		return NULL;
	}

	/* Make sure adc<n> is available for use */
	ret = os_mutex_get(&adc_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		ADC_LOG("failed to get mutex\r\n");
		return NULL;
	}

	/* Configure pinmux for external pins */
	if (channel <= ADC_CH7)
		switch (mdev_p->port_id) {
		case ADC0_ID:
			GPIO_PinMuxFun((GPIO_7 - channel), PINMUX_FUNCTION_6);
			break;

		case ADC1_ID:
			GPIO_PinMuxFun((GPIO_8 + channel), PINMUX_FUNCTION_6);
			break;
		}

	adc_data_p = (adc_data_t *) mdev_p->private_data;

	adc_data_p->channelx = channel;

	ADC_ChannelConfig(mdev_p->port_id, channel);

	ADC_Reset(mdev_p->port_id);

	ADC_Init(mdev_p->port_id, &adcConfig);

	ADC_Enable(mdev_p->port_id);

	ADC_IntMask(mdev_p->port_id, ADC_RDY, UNMASK);

	char sem_name[SEMAPHORE_NAME_LEN];

	snprintf(sem_name, sizeof(sem_name), "adc_sem_%d",
				 adc_id);

	ret = os_semaphore_create(&adc_sem[mdev_p->port_id],
                       sem_name);
	if (ret) {
		ADC_LOG("Failed to initialize semaphore for blocking"
			  " read\n\r");
		 return NULL;
	}

	os_semaphore_get(&adc_sem[mdev_p->port_id],
                      OS_WAIT_FOREVER);
	fill_cb_array();

	return mdev_p;
}

int adc_drv_result(mdev_t *mdev_p)
{
	int result = 0;
	ADC_ConversionStart(mdev_p->port_id);
	while(ADC_GetIntStatus(mdev_p->port_id, ADC_RDY) == RESET)
			;
	result = ADC_GetConversionResult(mdev_p->port_id);
	return result;
}

static void adcdrv_dma_irq_handler(int port_id)
{
	os_semaphore_put(&adc_sem[port_id]);
	return;
}

static void adc_set_dma_config(mdev_t *mdev_p, DMA_CFG_Type *dmaConfigSet, uint32_t addr, int num)
{
	DMA_Disable();

	DMA_IntClr(CHANNEL_0, INT_CH_ALL);

	DMA_ChannelCmd(CHANNEL_0, DISABLE);

	/* delay */
	int i;
	for (i = 0; i< 400; i++)
		;

	switch(mdev_p->port_id) {
	case ADC0_ID:
		dmaConfigSet->srcDmaAddr = (uint32_t) &ADC0->RESULT.WORDVAL;
		/* ADC0--hardware handshaking interface 8 */
		dmaConfigSet->srcHwHdskInterf = DMA_HW_HS_INTER_8;
		dmaConfigSet->destHwHdskInterf = DMA_HW_HS_INTER_8;
		/* dma handshaking map config, adc0--DMA_HS8_ADC0 */
		DMA_SetHandshakingMapping(DMA_HS8_ADC0);
		break;

	case ADC1_ID:
		dmaConfigSet->srcDmaAddr = (uint32_t) &ADC1->RESULT.WORDVAL;
		/*adc1--hardware handshaking interface 9*/
		dmaConfigSet->srcHwHdskInterf = DMA_HW_HS_INTER_9;
		dmaConfigSet->destHwHdskInterf = DMA_HW_HS_INTER_9;
		/* dma handshaking map config, adc1--DMA_HS9_ADC1 */
		DMA_SetHandshakingMapping(DMA_HS9_ADC1);
		break;
	}

	dmaConfigSet->destDmaAddr	   =  addr; 									 /* destination buffer address */
	dmaConfigSet->transfType       = DMA_PER_TO_MEM;                             /* Peripheral to memory */
	dmaConfigSet->srcBurstLength   = DMA_ITEM_1;                                 /* Src burst length: 1 item */
	dmaConfigSet->destBurstLength  = DMA_ITEM_1;                                 /* des burst length:1 item */
	dmaConfigSet->srcAddrInc       = DMA_ADDR_NOCHANGE;                          /* Src address nochange */
	dmaConfigSet->destAddrInc      = DMA_ADDR_INC;                               /* Des address increment */
	dmaConfigSet->srcTransfWidth   = DMA_TRANSF_WIDTH_16;                        /* Src transfer width 16 bits */
	dmaConfigSet->destTransfWidth  = DMA_TRANSF_WIDTH_16;                        /* Des transfer width 16 bit s*/
	dmaConfigSet->srcSwHwHdskSel   = DMA_HW_HANDSHAKING;                         /* Hardware handshaking interfac e*/
	dmaConfigSet->destSwHwHdskSel  = DMA_HW_HANDSHAKING;                         /* Hardware handshaking interface */
	dmaConfigSet->channelPriority  = DMA_CH_PRIORITY_0;                          /* Channel priority level 0 */
	dmaConfigSet->fifoMode         = DMA_FIFO_MODE_0;                            /* fifo mode 0 */

	/* dma reset*/
	DMA_Disable();

	DMA_IntClr(CHANNEL_0, INT_CH_ALL);

	/* dma channel initial, using channel0*/
	DMA_ChannelInit(CHANNEL_0, dmaConfigSet);

	/* dma tracnsfer data length config, dma config transfer width 16bit(2 byte) */
	if (num < MAX_DMA_BLK_SIZE) {
			DMA_SetTransfDataLength(CHANNEL_0, num);
	} else {
			DMA_SetBlkTransfSize(CHANNEL_0, MAX_DMA_BLK_SIZE); /* set block to MAX_DMA_BLK_SIZE */
	}

	DMA_Enable();

	/* dma channel0 enable*/
	DMA_ChannelCmd(CHANNEL_0, ENABLE);

	DMA_IntClr(CHANNEL_0, INT_CH_ALL);
	DMA_IntMask(CHANNEL_0, INT_DMA_TRANS_COMPLETE, UNMASK);

	install_int_callback(INT_DMA_CH0,
					  INT_DMA_TRANS_COMPLETE,
					  adc_drv_dma_irq_handler[mdev_p->port_id]);

	ADC_IntClr((ADC_ID_Type)mdev_p->port_id, ADC_RDY);
	ADC_DmaCmd((ADC_ID_Type)mdev_p->port_id, ENABLE);

	NVIC_SetPriority(DMA_IRQn, 0xf);
	NVIC_EnableIRQ(DMA_IRQn);
}

static void adc_reset_dma_config(mdev_t *mdev_p)
{
	/* Disable DMA interrupts */
	NVIC_DisableIRQ(DMA_IRQn);
	/* Un-install DMA callback */
	install_int_callback(INT_DMA_CH0,
					  INT_DMA_TRANS_COMPLETE, NULL);
	/* Disable ADC - DMA */
	ADC_DmaCmd((ADC_ID_Type)mdev_p->port_id, DISABLE);
	/* Disable DMA channel0 */
	DMA_ChannelCmd(CHANNEL_0 , DISABLE);
	/* Disable DMA Engine */
	DMA_Disable();
}

void adc_drv_timeout(ADC_ID_Type id, uint32_t tout)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_adc_name[id]);

	if (!mdev_p) {
		ADC_LOG("Unable to open device %s", mdev_adc_name[id]);
		return;
	}
	adc_tout[id] = tout;

}

int adc_drv_get_samples(mdev_t *mdev_p, uint16_t *buf, int num)
{
	if (num < 0)
		return -WM_FAIL;;

	/* start dma trasfer */
	adc_set_dma_config(mdev_p, &dmaconfig, (uint32_t) buf, num);

	int size = MAX_DMA_BLK_SIZE;
	int cnt = 1;

	while (num > 0) {
		ADC_LOG("samples to be transferred %d\r\n", num);
		if (num < MAX_DMA_BLK_SIZE)
			size = num;
		ADC_ConversionStop((ADC_ID_Type)mdev_adc->port_id);
		DMA_Disable();
		DMA_ChannelInit(CHANNEL_0, &dmaconfig);
		DMA_IntClr(CHANNEL_0, INT_CH_ALL);
		DMA_SetBlkTransfSize(CHANNEL_0, size);
		DMA_Enable();
		DMA_ChannelCmd(CHANNEL_0, 1);
		ADC_ConversionStart((ADC_ID_Type)mdev_adc->port_id);

		/* check if adc_tout (timeout) is specified */
		int sem_status;

		sem_status = os_semaphore_get(&adc_sem[mdev_p->port_id],
                     adc_tout[mdev_p->port_id]);

		if(sem_status != WM_SUCCESS)
			return -WM_FAIL;

		/* Check ADC state before stopping ADC. If conversion is on,
		 * wait till it is complete.
		 */
		while(ADC_GetIntStatus((ADC_ID_Type)mdev_adc->port_id, ADC_RDY) == RESET)
			os_thread_sleep(10);

		/* Increment DMA destination address */
		dmaconfig.destDmaAddr += size*2; /* each sample consumes 2 bytes */

		num = num - size;

		cnt++;
	}
	/* Reset ADC DMA configuration */
	adc_reset_dma_config(mdev_p);
	return WM_SUCCESS;
}


int adc_drv_close(mdev_t *mdev_p)
{
	ADC_ConversionStop(mdev_p->port_id);

	ADC_Disable(mdev_p->port_id);

	/* Delete semaphore */
	os_semaphore_delete(&adc_sem[mdev_p->port_id]);
	/* Mark adc<n> as free for use */
	return os_mutex_put(&adc_mutex[mdev_p->port_id]);
}

static void adc_drv_mdev_init(uint8_t adc_id)
{
	mdev_t *mdev_p;

	mdev_p = &mdev_adc[adc_id];

	mdev_p->name = mdev_adc_name[adc_id];

	mdev_p->port_id = adc_id;

	mdev_p->private_data = (uint32_t)&adcdrv_data[adc_id];
}

int adc_drv_init(ADC_ID_Type adc_id)
{
	/* Return if adc_id is invalid */
	if (adc_id < 0 || adc_id > MC200_ADC_NUM) {
		ADC_LOG("Port %d not enabled\r\n, port_id");
		return -WM_FAIL;
	}


	if (mdev_get_handle(mdev_adc_name[adc_id]) != NULL)
		return WM_SUCCESS;

	int ret = os_mutex_create(&adc_mutex[adc_id], mdev_adc_name[adc_id],
				      OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the adc mdev structure */
	adc_drv_mdev_init(adc_id);

	/* set timeout for ADC conversion + dma transfer
	 * if no timeout is specified, default value is set such that
	 * no timeout will occur.
	 */
	if (adc_tout[adc_id]== 0)
		adc_tout[adc_id] = OS_WAIT_FOREVER;

	/* Register the adc device driver for this port */
	mdev_register(&mdev_adc[adc_id]);

	return WM_SUCCESS;
}

int adc_drv_calib(mdev_t *dev, int16_t sysOffsetCalVal)
{
	if (dev == NULL) {
		ADC_LOG("Unable to open device\r\n");
		return -WM_FAIL;
	}
	int status = ADC_Calibration(dev->port_id, sysOffsetCalVal);

	if (!status)
		return -WM_FAIL;

	return WM_SUCCESS;

}

void adc_drv_deinit(ADC_ID_Type adc_id)
{
	/* Return if adc_id is invalid */
	if (adc_id < 0 || adc_id > MC200_ADC_NUM) {
		ADC_LOG("Port %d not enabled\r\n, port_id");
		return;
	}
	/* Return if the device was not registered */
	mdev_t *dev = mdev_get_handle(mdev_adc_name[adc_id]);
	if (dev == NULL) {
		ADC_LOG("Device was not initialized\r\n");
		return;
	}
	/* Delete mutex and de-register the device */
	os_mutex_delete(&adc_mutex[adc_id]);
	/* Deregister mdev handle */
	mdev_deregister(dev->name);
}
