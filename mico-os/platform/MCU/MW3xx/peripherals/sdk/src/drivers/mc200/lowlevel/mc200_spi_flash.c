/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <mc200_gpio.h>
#include <mc200_pinmux.h>
#include <mc200_clock.h>
#include <mc200_ssp.h>
#include <mc200_spi_flash.h>

static spi_flash_t ldata, *data = &ldata;

static void spi_pin_config(int id, bool cs)
{
	switch (id) {
	case SSP0_ID:
		GPIO_PinMuxFun(GPIO_32, GPIO32_SSP0_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_33, GPIO33_SSP0_FRM);
		GPIO_PinMuxFun(GPIO_34, GPIO34_SSP0_RXD);
		GPIO_PinMuxFun(GPIO_35, GPIO35_SSP0_TXD);
		break;
	case SSP1_ID:
		GPIO_PinMuxFun(GPIO_63, GPIO63_SSP1_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_64, GPIO64_SSP1_FRM);
		GPIO_PinMuxFun(GPIO_65, GPIO65_SSP1_RXD);
		GPIO_PinMuxFun(GPIO_66, GPIO66_SSP1_TXD);
		break;
	case SSP2_ID:
		GPIO_PinMuxFun(GPIO_40, GPIO40_SSP2_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_41, GPIO41_SSP2_FRM);
		GPIO_PinMuxFun(GPIO_42, GPIO42_SSP2_RXD);
		GPIO_PinMuxFun(GPIO_43, GPIO43_SSP2_TXD);
		break;
	}
}

static void spi_cs_activate()
{
	GPIO_WritePinOutput(data->cs, data->level);
}

static void spi_cs_deactivate()
{
	GPIO_WritePinOutput(data->cs, !data->level);
}

static int ssp_write(const uint8_t *buf, uint8_t *din, uint32_t num)
{
	uint32_t len = 0;

	while (num > 0) {
		/* Wait if fifo is full */
		while (SSP_GetStatus(data->port_id, SSP_STATUS_TFNF) != SET)
			;
		SSP_SendData(data->port_id, *buf);

		/* SSP_STATUS_BIT == SET when SSPx port is
		 * currently transmitting or receiving framed
		 * data.
		 */
		while (SSP_GetStatus(data->port_id, SSP_STATUS_BUSY) == SET)
			;

		/* SSP_STATUS_RFNE == SET when RXFIFO is non-empty */
		while (SSP_GetStatus(data->port_id, SSP_STATUS_RFNE) != SET)
			;

		if (din)
			*din++ = (uint8_t) SSP_RecvData(data->port_id);
		else
			SSP_RecvData(data->port_id);

		buf++;
		num--;
		len++;
	}
	return len;
}

static bool spi_send_cmd(spi_cmd_t *p_cmd)
{
	int len;
	int i;
	unsigned char dumy = 0xff;

	spi_cs_activate();

	/* Send command */
	len = ssp_write(p_cmd->cmd, NULL, p_cmd->cmd_size);
	if (len != p_cmd->cmd_size)
		goto err;

	/* Send dummy clocks */
	for (i = 0; i < p_cmd->dummy_size; i++) {
		len = ssp_write(&dumy, NULL, 1);
		if (len != 1)
			goto err;
	}
	if (p_cmd->data_size == 0)
		goto done;

	/* Read/Write data */
	for (i = 0; i < p_cmd->data_size; i++) {
		if (p_cmd->cmd_rx) {
			len = ssp_write(&dumy, &p_cmd->data[i], 1);
			if (len != 1)
				goto err;
		} else {
			len = ssp_write(&p_cmd->data[i], NULL, 1);
			if (len != 1)
				goto err;
		}
	}

done:
	spi_cs_deactivate();
	return true;

err:
	spi_cs_deactivate();
	return false;
}

/**
 * \brief Read status register of serial flash.
 * \param p_status Pointer to fill status result.
 * \return True if OK.
 */
static bool at25_read_status(uint8_t *p_status)
{
	at25_cmd_t at25_cmd = {
		.op_code = AT25_READ_STATUS,
	};

	spi_cmd_t spi_cmd = {
		.data = p_status,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 1,
		.dummy_size = 1,
		.data_size = 1
	};

	return spi_send_cmd(&spi_cmd);
}

/**
 * \brief disable serial flash write.
 * \return True if OK.
 */
static bool at25_disable_write()
{
	at25_cmd_t at25_cmd = {
		.op_code = AT25_WRITE_DISABLE,
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	return spi_send_cmd(&spi_cmd);
}

/**
 * \brief Enable serial flash write.
 * \return True if OK.
 */
static bool at25_enable_write()
{
	at25_cmd_t at25_cmd = {
		.op_code = AT25_WRITE_ENABLE,
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	return spi_send_cmd(&spi_cmd);
}

static bool at25_wait(uint8_t *p_status)
{
	uint8_t status = AT25_STATUS_RDYBSY;
	while (status & AT25_STATUS_RDYBSY) {
		if (at25_read_status(&status) == false)
			return false;
	}

	if (p_status)
		*p_status = status;
	return true;
}

int spi_flash_sleep()
{
	int ret;
	at25_cmd_t at25_cmd = {
		.op_code = AT25_DEEP_PDOWN,
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	ret = spi_send_cmd(&spi_cmd);
	if (ret == true)
		return DSUCCESS;
	else
		return DERROR;

}

int spi_flash_wakeup()
{
	int ret;
	at25_cmd_t at25_cmd = {
		.op_code = AT25_RES_DEEP_PDOWN,
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 1,
		.dummy_size = 3,
		.data_size = 0
	};

	ret = spi_send_cmd(&spi_cmd);
	if (ret == true)
		return DSUCCESS;
	else
		return DERROR;
}

int spi_flash_read_id(uint8_t *p_id)
{
	at25_cmd_t at25_cmd = {
		.op_code = AT25_READ_JEDEC_ID,
	};

	spi_cmd_t spi_cmd = {
		.data = (uint8_t *) p_id,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 1,
		.cmd_rx = 1,
		.dummy_size = 0,
		.data_size = 3
	};

	bool ret = spi_send_cmd(&spi_cmd);
	if (ret == true)
		return DSUCCESS;
	else
		return DERROR;
}


static int at25_erase_block_32k(uint32_t address)
{
	bool rc;
	uint8_t status;

	at25_cmd_t at25_cmd = {
		.op_code = AT25_BLOCK_ERASE_32K
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 4,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	at25_cmd.address_h = (address & 0xFF0000) >> 16;
	at25_cmd.address_m = (address & 0x00FF00) >> 8;
	at25_cmd.address_l = (address & 0x0000FF) >> 0;
	rc = at25_enable_write();
	if (!rc)
		return DERROR;

	rc = spi_send_cmd(&spi_cmd);
	if (!rc)
		return DERROR;

	rc = at25_wait(&status);
	if (!rc)
		return DERROR;
	if (status & AT25_STATUS_EPE)
		return DERROR;

	rc = at25_disable_write();
	if (!rc)
		return DERROR;
	return DSUCCESS;

}

static int at25_erase_block_64k(uint32_t address)
{
	bool rc;
	uint8_t status;

	at25_cmd_t at25_cmd = {
		.op_code = AT25_BLOCK_ERASE_64K
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 4,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	at25_cmd.address_h = (address & 0xFF0000) >> 16;
	at25_cmd.address_m = (address & 0x00FF00) >> 8;
	at25_cmd.address_l = (address & 0x0000FF) >> 0;
	rc = at25_enable_write();
	if (!rc)
		return DERROR;

	rc = spi_send_cmd(&spi_cmd);
	if (!rc)
		return DERROR;

	rc = at25_wait(&status);
	if (!rc)
		return DERROR;
	if (status & AT25_STATUS_EPE)
		return DERROR;

	rc = at25_disable_write();
	if (!rc)
		return DERROR;
	return DSUCCESS;

}

static int at25_erase_block_4k(uint32_t address)
{
	bool rc;
	uint8_t status;

	at25_cmd_t at25_cmd = {
		.op_code = AT25_BLOCK_ERASE_4K
	};

	spi_cmd_t spi_cmd = {
		.data = NULL,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 4,
		.cmd_rx = 0,
		.dummy_size = 0,
		.data_size = 0
	};

	at25_cmd.address_h = (address & 0xFF0000) >> 16;
	at25_cmd.address_m = (address & 0x00FF00) >> 8;
	at25_cmd.address_l = (address & 0x0000FF) >> 0;
	rc = at25_enable_write();
	if (!rc)
		return DERROR;

	rc = spi_send_cmd(&spi_cmd);
	if (!rc)
		return DERROR;

	rc = at25_wait(&status);
	if (!rc)
		return DERROR;
	if (status & AT25_STATUS_EPE)
		return DERROR;

	rc = at25_disable_write();
	if (!rc)
		return DERROR;
	return DSUCCESS;
}

int spi_flash_eraseall()
{
	int ret;
	uint32_t i, address = 0x0, size_64k = 0x10000;
	for (i = 0; i < (DATAFLASH_TOTAL_SIZE / size_64k); i++) {
		ret = at25_erase_block_64k(address);
		if (ret != DSUCCESS)
			break;
		address += size_64k;
	}
	return ret;
}

int spi_flash_erase(uint32_t address, uint32_t end_address)
{
	int ret;
	uint32_t length, valid_start;

	length = end_address - address + 1;

	while (length != 0) {
		if ((address & (AT25_64K_BLOCK_SIZE - 1)) == 0 &&
			length > (AT25_64K_BLOCK_SIZE - AT25_SECTOR_SIZE)) {
			/* Address is a multiple of 64K and length is >
			 * (64K block -4K sector)
			 * So directly erase 64K from this address */
			ret = at25_erase_block_64k(address);
			end_address = address + AT25_64K_BLOCK_SIZE;
		} else if ((address & (AT25_32K_BLOCK_SIZE - 1)) == 0 &&
			length > (AT25_32K_BLOCK_SIZE - AT25_SECTOR_SIZE)) {
			/* Address is a multiple of 32K and length is >
			 * (32K block -4K sector)
			 * So directly erase 32K from this address */
			ret = at25_erase_block_32k(address);
			end_address = address + AT25_32K_BLOCK_SIZE;
		} else {
			/* Find 4K aligned address and erase 4K sector */
			valid_start = address - (address &
					(AT25_SECTOR_SIZE - 1));
			ret = at25_erase_block_4k(valid_start);
			end_address = valid_start + AT25_SECTOR_SIZE;
		}

		/* If erase operation fails then return error */
		if (ret != DSUCCESS)
			return DERROR;

		/* Calculate the remaining length that is to be erased yet */
		if (length < (end_address - address))
			length = 0;
		else
			length -= (end_address - address);
		address = end_address;

	}
	return DSUCCESS;
}

int spi_flash_erase_block_4K(uint32_t address)
{
	return at25_erase_block_4k(address);
}

int spi_flash_erase_block_32K(uint32_t address)
{
	return at25_erase_block_32k(address);
}

int spi_flash_erase_block_64K(uint32_t address)
{
	return at25_erase_block_64k(address);
}

#define Min(a, b)           (((a) < (b)) ?  (a) : (b))
int spi_flash_write(uint8_t *p_buf, uint32_t size, uint32_t address)
{
	bool rc;
	uint8_t status;
	uint32_t write_size;

	/* Previous check: (size <= 0)||(size > 20*1024) */
	if (size <= 0)
		return DERROR;

	while (size > 0) {
		write_size =
		    Min(size,
			DATAFLASH_PAGE_SIZE - (address % DATAFLASH_PAGE_SIZE));

		at25_cmd_t at25_cmd = {
			.op_code = AT25_BYTE_PAGE_PROGRAM
		};

		spi_cmd_t spi_cmd = {
			.data = p_buf,
			.cmd = (uint8_t *) &at25_cmd,
			.cmd_size = 4,
			.cmd_rx = 0,
			.dummy_size = 0,
			.data_size = write_size
		};

		at25_cmd.address_h = (address & 0xFF0000) >> 16;
		at25_cmd.address_m = (address & 0x00FF00) >> 8;
		at25_cmd.address_l = (address & 0x0000FF) >> 0;
		rc = at25_enable_write();
		if (!rc)
			return DERROR;

		rc = spi_send_cmd(&spi_cmd);
		if (!rc)
			return DERROR;

		rc = at25_wait(&status);
		if (!rc)
			return DERROR;

		if (status & AT25_STATUS_EPE)
			return DERROR;

		p_buf += write_size;
		size -= write_size;
		address += write_size;
	}
	return DSUCCESS;
}

int spi_flash_read(uint8_t *p_buf, uint32_t size, uint32_t address)
{
	bool rc;

	at25_cmd_t at25_cmd = {
		.op_code = AT25_READ_ARRAY
	};

	if ((size + address) > DATAFLASH_TOTAL_SIZE)
		return DERROR;

	spi_cmd_t spi_cmd = {
		.data = p_buf,
		.cmd = (uint8_t *) &at25_cmd,
		.cmd_size = 4,
		.cmd_rx = 1,
		.dummy_size = 1,
		.data_size = size
	};

	at25_cmd.address_h = (address & 0xFF0000) >> 16;
	at25_cmd.address_m = (address & 0x00FF00) >> 8;
	at25_cmd.address_l = (address & 0x0000FF) >> 0;

	rc = spi_send_cmd(&spi_cmd);
	if (!rc)
		return DERROR;

	return DSUCCESS;

}

static void at25_flash_init()
{
#define ACTIVE_LOW 0
	data->port_id = SSP1_ID;	/* SSP Port 1 */
	data->cs = 64;			/* Chip select GPIO 64 */
	data->level = ACTIVE_LOW;	/* Chip select level */
}

int spi_flash_init()
{
	SSP_CFG_Type sspCfgStruct;
	SSP_FIFO_Type sspFifoCfg;
	SPI_Param_Type spiParaStruct;

	/* Initialize AT25L028 Flash specific parameters */
	at25_flash_init();

	/* Default SSP Frequency: 10000000 */
	/* board_cpu_freq() : 200000000 */
	uint16_t divider = (uint32_t) (200000000 / 10000000);

	switch (data->port_id) {
	case SSP0_ID:
		CLK_ModuleClkEnable(CLK_SSP0);
		CLK_SSPClkSrc(CLK_SSP_ID_0, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP0, divider);
		break;
	case SSP1_ID:
		CLK_ModuleClkEnable(CLK_SSP1);
		CLK_SSPClkSrc(CLK_SSP_ID_1, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP1, divider);
		break;
	case SSP2_ID:
		CLK_ModuleClkEnable(CLK_SSP2);
		CLK_SSPClkSrc(CLK_SSP_ID_2, CLK_SYSTEM);
		CLK_ModuleClkDivider(CLK_SSP2, divider);
		break;
	}

	/* Configure the pinmux for ssp pins */
	if (data->cs >= 0) {
		spi_pin_config(data->port_id, 0);
		/* Use user specified chip select pin */
		GPIO_PinMuxFun(data->cs, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(data->cs, GPIO_OUTPUT);
		/* Initially keep slave de-selected */
		GPIO_WritePinOutput(data->cs, !data->level);
	} else {
		spi_pin_config(data->port_id, 1);
	}

	/* Configure SSP interface */
	sspCfgStruct.mode = SSP_NORMAL;
	sspCfgStruct.masterOrSlave = SSP_MASTER;
	sspCfgStruct.trMode = SSP_TR_MODE;
	sspCfgStruct.dataSize = SSP_DATASIZE_8;
	sspCfgStruct.sfrmPola = SSP_SAMEFRM_PSP;
	sspCfgStruct.slaveClkRunning = SSP_SLAVECLK_TRANSFER;
	sspCfgStruct.txd3StateEnable = ENABLE;
	/* RXFIFO inactivity timeout, [timeout = 100 / 26MHz] (Bus clock) */
	sspCfgStruct.timeOutVal = 100;

	sspCfgStruct.frameFormat = SSP_FRAME_SPI;
	sspCfgStruct.txd3StateType = SSP_TXD3STATE_ELSB;

	/* Configure SSP Fifo */
	sspFifoCfg.fifoPackMode =  DISABLE;

	/* SSP_FIFO_RX_THRESHOLD */
	sspFifoCfg.rxFifoFullLevel = 0;
	/* SSP_FIFO_TX_THRESHOLD */
	sspFifoCfg.txFifoEmptyLevel = 0;
	sspFifoCfg.rxDmaService = DISABLE;
	sspFifoCfg.txDmaService = DISABLE;
	sspCfgStruct.trailByte = SSP_TRAILBYTE_CORE;

	/* Let the settings take effect */
	SSP_Disable(data->port_id);
	SSP_Init(data->port_id, &sspCfgStruct);
	SSP_FifoConfig(data->port_id, &sspFifoCfg);

	/* Do frame format config */
	spiParaStruct.spiClkPhase = SPI_SCPHA_1;
	spiParaStruct.spiClkPolarity = SPI_SCPOL_LOW;
	SPI_Config(data->port_id, &spiParaStruct);

	SSP_Enable(data->port_id);
	return 0;
}
