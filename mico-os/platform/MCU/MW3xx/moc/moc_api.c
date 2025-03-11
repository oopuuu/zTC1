#include "common.h"
#include "moc_api.h"

extern const mico_api_t *lib_api_p;

extern platform_spi_driver_t            platform_spi_drivers[];
extern platform_i2c_driver_t            platform_i2c_drivers[];


/* WIFI MGR */
OSStatus StartNetwork(network_InitTypeDef_st* inNetworkInitPara)
{
	return lib_api_p->micoWlanStart(inNetworkInitPara);
}
OSStatus StartAdvNetwork(network_InitTypeDef_adv_st* inNetworkInitParaAdv)
{
	return lib_api_p->micoWlanStartAdv(inNetworkInitParaAdv);
}
OSStatus getNetPara(IPStatusTypedef *outNetpara, WiFi_Interface inInterface)
{
	return lib_api_p->micoWlanGetIPStatus(outNetpara, inInterface);
}
OSStatus CheckNetLink(LinkStatusTypeDef *outStatus)
{
	return lib_api_p->micoWlanGetLinkStatus(outStatus);
}
void mxchipStartScan(void)
{
	lib_api_p->micoWlanStartScan();
}
void mxchipStartAdvScan(void)
{
	lib_api_p->micoWlanStartScanAdv();
}
OSStatus wifi_power_down(void)
{
	return lib_api_p->micoWlanPowerOff();
}
OSStatus wifi_power_up(void)
{
	return lib_api_p->micoWlanPowerOn();
}
OSStatus wlan_disconnect(void)
{
	return lib_api_p->micoWlanSuspend();
}
OSStatus sta_disconnect(void)
{
	return lib_api_p->micoWlanSuspendStation();
}
OSStatus uap_stop(void)
{
	return lib_api_p->micoWlanSuspendSoftAP();
}
OSStatus OpenEasylink2_withdata(int inTimeout)
{
	return lib_api_p->micoWlanStartEasyLink(inTimeout);
}
OSStatus OpenEasylink(int inTimeout)
{
	return lib_api_p->micoWlanStartEasyLinkPlus(inTimeout);
}
OSStatus CloseEasylink2(void)
{
	return lib_api_p->micoWlanStopEasyLink();
}
OSStatus CloseEasylink(void)
{
	return lib_api_p->micoWlanStopEasyLinkPlus();
}
OSStatus OpenConfigmodeWPS(int inTimeout)
{
	return lib_api_p->micoWlanStartWPS(inTimeout);
}
OSStatus CloseConfigmodeWPS(void)
{
	return lib_api_p->micoWlanStopWPS();
}
OSStatus OpenAirkiss(int inTimeout)
{
	return lib_api_p->micoWlanStartAirkiss(inTimeout);
}
OSStatus CloseAirkiss(void)
{
	return lib_api_p->micoWlanStopAirkiss();
}
void ps_enable(void)
{
	lib_api_p->micoWlanEnablePowerSave();
}
void ps_disable(void)
{
	lib_api_p->micoWlanDisablePowerSave();
}
void wifimgr_debug_enable(bool enable)
{
	lib_api_p->wifimgr_debug_enable(enable);
}

int mico_wlan_monitor_no_easylink(void)
{
	return lib_api_p->mico_wlan_monitor_no_easylink();
}


int mico_wlan_monitor_rx_type(int type)
{
	return lib_api_p->mico_wlan_monitor_rx_type(type);
}
int mico_wlan_start_monitor(void)
{
	return lib_api_p->mico_wlan_start_monitor();
}
int mico_wlan_stop_monitor(void)
{
	return lib_api_p->mico_wlan_stop_monitor();
}
int mico_wlan_set_channel(uint8_t channel)
{
	return lib_api_p->mico_wlan_set_channel((int)channel);
}
void mico_wlan_register_monitor_cb(monitor_cb_t fn)
{
	lib_api_p->mico_wlan_register_monitor_cb(fn);
}

int mxchip_active_scan(char*ssid, int is_adv)
{
	return lib_api_p->mxchip_active_scan(ssid, is_adv);
}

void wlan_set_channel(int channel)
{
    lib_api_p->wlan_set_channel(channel);
}


/* HAL: GPIO
{
	return lib_api_p->;
} FLASH
{
	return lib_api_p->;
} UART */
mico_logic_partition_t* MicoFlashGetInfo( mico_partition_t inPartition )
{
	return lib_api_p->MicoFlashGetInfo(inPartition);
}
OSStatus MicoFlashErase(mico_partition_t inPartition, uint32_t off_set, uint32_t size)
{
	return lib_api_p->MicoFlashErase(inPartition, off_set, size);
}
OSStatus MicoFlashWrite( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength)
{
	 lib_api_p->MicoFlashWrite(inPartition, off_set, inBuffer, inBufferLength);
	 return 0;
}
OSStatus MicoFlashRead( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer, uint32_t inBufferLength)
{
	return lib_api_p->MicoFlashRead(inPartition, off_set, outBuffer, inBufferLength);
}
OSStatus MicoFlashEnableSecurity( mico_partition_t partition, uint32_t off_set, uint32_t size )
{
	return lib_api_p->MicoFlashEnableSecurity(partition, off_set, size );
}

OSStatus MicoGpioInitialize( mico_gpio_t gpio, mico_gpio_config_t configuration )
{
	return lib_api_p->MicoGpioInitialize(gpio, configuration );
}
OSStatus MicoGpioFinalize( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioFinalize(gpio);
}
OSStatus MicoGpioOutputHigh( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioOutputHigh(gpio);
}
OSStatus MicoGpioOutputLow( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioOutputLow(gpio);
}
OSStatus MicoGpioOutputTrigger( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioOutputTrigger(gpio);
}
bool MicoGpioInputGet( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioInputGet(gpio);
}
OSStatus MicoGpioEnableIRQ( mico_gpio_t gpio, mico_gpio_irq_trigger_t trigger, mico_gpio_irq_handler_t handler, void* arg )
{
	return lib_api_p->MicoGpioEnableIRQ(gpio, trigger, handler, arg );
}
OSStatus MicoGpioDisableIRQ( mico_gpio_t gpio )
{
	return lib_api_p->MicoGpioDisableIRQ(gpio);
}

OSStatus MicoUartInitialize( mico_uart_t uart, const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer )
{
	return lib_api_p->MicoUartInitialize(uart, config, optional_rx_buffer );
}
OSStatus MicoUartFinalize( mico_uart_t uart )
{
	return lib_api_p->MicoUartFinalize(uart);
}
OSStatus MicoUartSend( mico_uart_t uart, const void* data, uint32_t size )
{
	return lib_api_p->MicoUartSend(uart, data, size );
}
OSStatus MicoUartRecv( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout )
{
	return lib_api_p->MicoUartRecv(uart, data, size, timeout );
}
uint32_t MicoUartGetLengthInBuffer( mico_uart_t uart )
{
	return lib_api_p->MicoUartGetLengthInBuffer(uart);
}
OSStatus MicoUartPinRedirect( mico_uart_t uart )
{
	lib_api_p->MicoUartPinRedirect(uart);
	return kNoErr;
}

void wifistate_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->wifistate_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}

void wifidebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->wifidebug_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void wifiscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->wifiscan_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}

void ifconfig_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->ifconfig_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void arp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->arp_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void ping_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->ping_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void dns_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->dns_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void task_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->task_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void socket_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->socket_show_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void memory_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->memory_show_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void memory_dump_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->memory_dump_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void memory_set_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->memory_set_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void memp_dump_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->memp_dump_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}
void driver_state_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	lib_api_p->driver_state_Command(pcWriteBuffer, xWriteBufferLen, argc, argv);
}

int wlan_driver_version( char* outVersion, uint8_t inLength )
{
	return lib_api_p->wlan_driver_version(outVersion, inLength);
}

void mico_wlan_get_mac_address( uint8_t *mac )
{
	lib_api_p->wlan_get_mac_address(mac);
}

void InitMd5(md5_context*md5)
{
	lib_api_p->InitMd5(md5);
}
void Md5Update(md5_context* md5, unsigned char *input, int ilen)
{
	lib_api_p->Md5Update(md5, input, ilen);
}
void Md5Final(md5_context* md5, uint8_t* hash)
{
	lib_api_p->Md5Final(md5, hash);
}
int Md5Hash(const uint8_t* data, uint32_t len, uint8_t* hash)
{
	return lib_api_p->Md5Hash(data, len, hash);
}
void AesEncryptDirect(Aes* aes, uint8_t* out, const uint8_t* in)
{
	lib_api_p->AesEncryptDirect(aes, out, in);
}
void AesDecryptDirect(Aes* aes, uint8_t* out, const uint8_t* in)
{
	lib_api_p->AesDecryptDirect(aes, out, in);
}
//int  AesSetKeyDirect(Aes* aes, const byte* key, word32 ilen,const byte* out, int dir);
int AesSetKeyDirect(Aes* aes, const unsigned char* key, unsigned int len, const unsigned char* iv, int dir)
{
	return lib_api_p->AesSetKeyDirect(aes, key, len, iv, dir);
}
int aes_encrypt(int sz, const char * key, const char * in, char * out)
{
	return lib_api_p->aes_encrypt(sz, key, in, out);
}
int aes_decrypt(int sz, const char * key, const char * in, char * out)
{
	return lib_api_p->aes_decrypt(sz, key, in, out);
}
int  AesSetKey(Aes* aes, const uint8_t* key, unsigned int  len,
                          const uint8_t* iv, int dir)
{
	return lib_api_p->AesSetKey(aes, key, len, iv, dir);
}
int  AesSetIV(Aes* aes, const uint8_t* iv)
{
	return lib_api_p->AesSetIV(aes, iv);
}
int  AesCbcEncrypt(Aes* aes, uint8_t* out,
                              const uint8_t* in, unsigned int sz)
{
	return lib_api_p->AesCbcEncrypt(aes, out, in, sz);
}
int  AesCbcDecrypt(Aes* aes, uint8_t* out,
                              const uint8_t* in, unsigned int sz){
	return lib_api_p->AesCbcDecrypt(aes, out, in, sz);
}

OSStatus mxchipInit(void)
{
	lib_api_p->mxchipInit();
	return kNoErr;
}

micoMemInfo_t* mico_memory_info(void)
{
	return lib_api_p->mico_memory_info();
}
char* system_lib_version(void)
{
    return lib_api_p->library_version;
}

void MicoSystemReboot(void)
{
	lib_api_p->MicoSystemReboot();
}

char *mico_get_bootloader_ver(void)
{
	return "bootloader";
}

void MicoSystemStandBy( uint32_t secondsToWakeup )
{
  lib_api_p->wifi_off_mcu_standby(secondsToWakeup);
}

char *get_ali_key(void)
{
	return lib_api_p->get_ali_key();
}

char *get_ali_secret(void)
{
	return lib_api_p->get_ali_secret();
}

void MicoRtcInitialize(void)
{
	lib_api_p->MicoRtcInitialize();
}

OSStatus MicoRtcGetTime(mico_rtc_time_t* time)
{
	int ret =  lib_api_p->MicoRtcGetTime(time);

	time->month += 1;
	time->year -= 100;
	return ret;
}

OSStatus MicoRtcSetTime(mico_rtc_time_t* time)
{
	time->month -= 1;
	time->year += 100;
	return lib_api_p->MicoRtcSetTime(time);
}

#if defined ( __ICCARM__ )
struct tm *localtime(const time_t * time)
{
	return lib_api_p->localtime(time);
}
#endif

char *asctime(const struct tm *tm)
{
	return lib_api_p->asctime(tm);
}

int wifi_set_country(int country)
{
	return lib_api_p->wifi_set_country(country);
}

int switch_active_firmware(void)
{
	return lib_api_p->switch_active_firmrware();
}

int get_last_reset_reason(void)
{
	return lib_api_p->last_reset_reason();
}

void system_config_set(mico_system_config_t *cfg)
{
	lib_api_p->system_config_set(cfg);
}

mico_system_config_t *system_config_get(void)
{
	return lib_api_p->system_config_get();
}

int aon_write( uint32_t offset, uint8_t* in ,uint32_t len)
{
	return lib_api_p->aon_write(offset, in, len);
}

int aon_read( uint32_t offset, uint8_t* out, uint32_t len)
{
	return lib_api_p->aon_read(offset, out, len);
}


int lwip_ioctl(int s, long cmd, void *argp)
{
	return lib_api_p->lwip_apis->lwip_ioctl(s, cmd, argp);
}
int lwip_fcntl(int s, int cmd, int val)
{
	return lib_api_p->lwip_apis->lwip_fcntl(s, cmd, val);
}

void lwip_freeaddrinfo(struct addrinfo *ai)
{
	lib_api_p->lwip_apis->lwip_freeaddrinfo(ai);
}

int lwip_getaddrinfo(const char *nodename,
	   const char *servname,
	   const struct addrinfo *hints,
	   struct addrinfo **res)
{
	return lib_api_p->lwip_apis->lwip_getaddrinfo(nodename,
	   			servname, hints, res);
}

char * ipaddr_ntoa(const ip_addr_t *addr)
{
	return lib_api_p->lwip_apis->ipaddr_ntoa(addr);
}
uint32_t ipaddr_addr(const char *cp)
{
	return lib_api_p->lwip_apis->ipaddr_addr(cp);
}

uint16_t
lwip_htons(uint16_t n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/**
 * Convert an uint16_t from network- to host byte order.
 *
 * @param n uint16_t in network byte order
 * @return n in host byte order
 */
uint16_t
lwip_ntohs(uint16_t n)
{
  return lwip_htons(n);
}

/**
 * Convert an uint32_t from host- to network byte order.
 *
 * @param n uint32_t in host byte order
 * @return n in network byte order
 */
uint32_t
lwip_htonl(uint32_t n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

/**
 * Convert an uint32_t from network- to host byte order.
 *
 * @param n uint32_t in network byte order
 * @return n in host byte order
 */
uint32_t
lwip_ntohl(uint32_t n)
{
  return lwip_htonl(n);
}

OSStatus MicoPwmInitialize(mico_pwm_t pwm, uint32_t freequency, float duty_cycle)
{
	return lib_api_p->pwm_apis->pwm_init(pwm, freequency, duty_cycle);
}

OSStatus MicoPwmStart(mico_pwm_t pwm)
{
	return lib_api_p->pwm_apis->pwm_start(pwm);
}

OSStatus MicoPwmStop(mico_pwm_t pwm)
{
	return lib_api_p->pwm_apis->pwm_stop(pwm);
}

OSStatus MicoWdgInitialize( uint32_t timeout )
{
	return lib_api_p->wdg_apis->wdg_init(timeout);
}

void MicoWdgReload( void )
{
	lib_api_p->wdg_apis->wdg_reload();
}

OSStatus MicoWdgFinalize( void )
{
	return lib_api_p->wdg_apis->wdg_stop();
}

int Cyassl_get_fd(const void *ssl)
{
	return lib_api_p->ssl_get_fd(ssl);
}

OSStatus MicoAdcInitialize( mico_adc_t adc, uint32_t sampling_cycle )
{
	return lib_api_p->adc_apis->MicoAdcInitialize(adc, sampling_cycle);
}

OSStatus  MicoAdcFinalize( mico_adc_t adc )
{
    return lib_api_p->adc_apis->MicoAdcFinalize(adc);
}

OSStatus MicoAdcTakeSample( mico_adc_t adc, uint16_t* output )
{
    return lib_api_p->adc_apis->MicoAdcTakeSample(adc, output);
}

OSStatus MicoAdcTakeSampleStreram( mico_adc_t adc, void* buffer, uint16_t buffer_length )
{
    return lib_api_p->adc_apis->MicoAdcTakeSampleStreram(adc, buffer, buffer_length);
}

OSStatus MicoI2cInitialize( mico_i2c_device_t* device )
{
    OSStatus result;

    if( platform_i2c_drivers[device->port].i2c_mutex == NULL)
      mico_rtos_init_mutex( &platform_i2c_drivers[device->port].i2c_mutex );

    mico_rtos_lock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
    result = lib_api_p->i2c_apis->i2c_init(device);
    mico_rtos_unlock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
    return result;
}

OSStatus MicoI2cFinalize( mico_i2c_device_t* device )
{
    if( platform_i2c_drivers[device->port].i2c_mutex != NULL){
      mico_rtos_deinit_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
      platform_i2c_drivers[device->port].i2c_mutex = NULL;
    }

    return lib_api_p->i2c_apis->i2c_deinit(device);
}

bool MicoI2cProbeDevice( mico_i2c_device_t* device, int retries )
{
    bool ret;

    mico_rtos_lock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
    ret = lib_api_p->i2c_apis->i2c_probe_device(device, retries);
    mico_rtos_unlock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
    return ret;
}

OSStatus MicoI2cBuildTxMessage( mico_i2c_message_t* message, const void* tx_buffer, uint16_t  tx_buffer_length, uint16_t retries )
{
    return lib_api_p->i2c_apis->i2c_build_tx_msg(message, tx_buffer, tx_buffer_length, retries );
}

OSStatus MicoI2cBuildRxMessage( mico_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries )
{
	return lib_api_p->i2c_apis->i2c_build_rx_msg(message, rx_buffer, rx_buffer_length, retries );
}

OSStatus MicoI2cBuildCombinedMessage( mico_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries )
{
	return lib_api_p->i2c_apis->i2c_build_combined_msg(message, tx_buffer, rx_buffer,
										tx_buffer_length, rx_buffer_length, retries );
}

OSStatus MicoI2cTransfer( mico_i2c_device_t* device, mico_i2c_message_t* messages, uint16_t number_of_messages )
{
    OSStatus err = kNoErr;

    mico_rtos_lock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );
    err = lib_api_p->i2c_apis->i2c_transfer(device, messages, number_of_messages);
    mico_rtos_unlock_mutex( &platform_i2c_drivers[device->port].i2c_mutex );

    return err;
}

OSStatus MicoSpiInitialize( const mico_spi_device_t* spi )
{
    if ( platform_spi_drivers[spi->port].initialized == MICO_TRUE )
        return kNoErr;
    lib_api_p->spi_apis->spi_init(spi);
    platform_spi_drivers[spi->port].initialized = MICO_TRUE;
    return kNoErr;
}

OSStatus MicoSpiFinalize( const mico_spi_device_t* spi )
{
    return lib_api_p->spi_apis->spi_finalize(spi);
}

OSStatus MicoSpiTransfer( const mico_spi_device_t* spi, const mico_spi_message_segment_t* segments, uint16_t number_of_segments )
{
	return lib_api_p->spi_apis->spi_transfer(spi, segments, number_of_segments );
}


OSStatus MicoRandomNumberRead( void *inBuffer, int inByteCount )
{
    lib_api_p->get_random_sequence(inBuffer, inByteCount);
	return kNoErr;
}

uint32_t RNG_GetRandomNumber(void)
{
    uint32_t d;
    MicoRandomNumberRead((unsigned char*)&d, 4);
    return d;
}

int wlan_inject_frame(const uint8_t *buff, size_t len)
{
	return lib_api_p->wlan_inject_frame(buff, len);
}

OSStatus mico_wlan_send_mgnt(uint8_t *buffer, uint32_t length)
{
	// I don't know the return value;
	lib_api_p->wlan_inject_frame(buffer, length);
	return kNoErr;
}

void MicoMcuPowerSaveConfig(int enable)
{
}

/**
 * This API can be used to start/stop the management frame forwards
 * to host through datapath.
 *
 * \param[in] bss_type The interface from which management frame needs to be
 *	   collected.
 * \param[in] mgmt_subtype_mask     Management Subtype Mask
 *	      If Bit X is set in mask, it means that IEEE Management Frame
 *	      SubTyoe X is to be filtered and passed through to host.
 *            Bit                   Description
 *	      [31:14]               Reserved
 *	      [13]                  Action frame
 *	      [12:9]                Reserved
 *	      [8]                   Beacon
 *	      [7:6]                 Reserved
 *	      [5]                   Probe response
 *	      [4]                   Probe request
 *	      [3]                   Reassociation response
 *	      [2]                   Reassociation request
 *	      [1]                   Association response
 *	      [0]                   Association request
 *	      Support multiple bits set.
 *	      0 = stop forward frame
 *	      1 = start forward frame
 *
 *\param[in] rx_mgmt_callback The receive callback where the received management
 *	  frames are passed.

 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if command fails.
 *
 * \note Pass Management Subtype Mask all zero to disable all the management
 * 	 frame forward to host.
 */
int wlan_rx_mgmt_indication(const enum wlan_bss_type bss_type,
			const uint32_t mgmt_subtype_mask,
			void (*rx_mgmt_callback)(const enum wlan_bss_type
				bss_type, const uint8_t *frame,
				const uint16_t len))
{
	return lib_api_p->wlan_rx_mgmt_indication(
		bss_type, mgmt_subtype_mask, rx_mgmt_callback);
}


int wlan_remain_on_channel(const bool status, const uint8_t channel,
				const uint32_t ms)
{
	return lib_api_p->wlan_remain_on_channel(status, channel, ms);
}

int wifi_bridge_mode_enable(bool hidden_ssid)
{
	return lib_api_p->wifi_bridge_mode_enable(hidden_ssid);
}

int wifi_bridge_mode_disable(void)
{
	return lib_api_p->wifi_bridge_mode_disable();
}

int send_easylink_minus(uint32_t ip, char *ssid, char *key)
{
	return lib_api_p->send_easylink_minus(ip, ssid, key);
}


