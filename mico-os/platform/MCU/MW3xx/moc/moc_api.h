#ifndef _MICO_API_H_
#define _MICO_API_H_

#include "lwip_api_define.h"
#include "mico.h"

#define INTERFACE_VERSION 1

typedef void (*ssl_Logging_cb)( const int logLevel,
                                const char * const logMessage );

typedef enum
{
    /** US FCC */
    COUNTRY_US = 1,
    /** IC Canada */
    COUNTRY_CA,
    /** Singapore */
    COUNTRY_SG,
    /** ETSI */
    COUNTRY_EU,
    /** Australia */
    COUNTRY_AU,
    /** Republic Of Korea */
    COUNTRY_KR,
    /** France */
    COUNTRY_FR,
    /** Japan */
    COUNTRY_JP,
    /** China */
    COUNTRY_CN,
} country_code_t;
enum wlan_bss_type
{
    WLAN_BSS_TYPE_STA = 0,
    WLAN_BSS_TYPE_UAP = 1,
    WLAN_BSS_TYPE_WIFIDIRECT = 2,
    WLAN_BSS_TYPE_ANY = 0xff,
};
typedef enum
{
    ASSOC_REQ_FRAME = 0x00,
    ASSOC_RESP_FRAME = 0x10,
    REASSOC_REQ_FRAME = 0x20,
    REASSOC_RESP_FRAME = 0x30,
    PROBE_REQ_FRAME = 0x40,
    PROBE_RESP_FRAME = 0x50,
    BEACON_FRAME = 0x80,
    DISASSOC_FRAME = 0xA0,
    AUTH_FRAME = 0xB0,
    DEAUTH_FRAME = 0xC0,
    ACTION_FRAME = 0xD0,
    DATA_FRAME = 0x08,
    QOS_DATA_FRAME = 0x88,
} wifi_frame_type_t;

/** 802_11_header packet */
typedef struct _wifi_mgmt_frame_t
{
    /** Packet Length */
    uint16_t frm_len;
    /** Frame Type */
    wifi_frame_type_t frame_type;
    /** Frame Control flags */
    uint8_t frame_ctrl_flags;
    /** Duration ID */
    uint16_t duration_id;
    /** Address1 */
    uint8_t addr1[6];
    /** Address2 */
    uint8_t addr2[6];
    /** Address3 */
    uint8_t addr3[6];
    /** Sequence Control */
    uint16_t seq_ctl;
    /** Address4 */
    uint8_t addr4[6];
    /** Frame payload */
    uint8_t payload[0];
} wlan_mgmt_frame_t;

typedef struct
{
    OSStatus (*pwm_init)( mico_pwm_t pwm, uint32_t frequency, float duty_cycle );
    OSStatus (*pwm_start)( mico_pwm_t pwm );
    OSStatus (*pwm_stop)( mico_pwm_t pwm );
} pwm_api_t;

typedef struct
{
    OSStatus (*wdg_init)( uint32_t timeout );
    void (*wdg_reload)( void );
    OSStatus (*wdg_stop)( void );
} wdg_api_t;

#define LAST_RST_CAUSE_VBAT    (1<<0)
#define LAST_RST_CAUSE_AV12    (1<<1)
#define LAST_RST_CAUSE_AV18    (1<<2)
#define LAST_RST_CAUSE_SOFTRST (1<<3)
#define LAST_RST_CAUSE_LOCKUP  (1<<4)
#define LAST_RST_CAUSE_WDT     (1<<5)

#define USER_APP_ADDR 0x1f064000 /* 400KB offset */
#define USER_MAGIC_NUM 0xC89346

#define time_t unsigned long
/** Power States of MCU */
typedef enum
{

    /** (Active Mode): This is the full power state of MCU.
     *  Instruction execution takes place only in PM0.
     */
    PM0,
    /** (Idle Mode): In this mode Cortex M3 core function
     *  clocks are stopped until the occurrence of any interrupt.
     *  This consumes lower power than PM0. */
    PM1,

    /** (Standby Mode):In this mode, the Cortex M3,
     *  most of the peripherals & SRAM arrays are in
     *  low-power mode.The PMU and RTC are operational.
     *  A wakeup can happen by timeout (RTC based) or by asserting the
     *  WAKEUP 0/1 lines.This consumes much lower power than PM1.
     */
    PM2,

    /**(Sleep Mode): This mode further aggressively conserves power.
     * Only 192 KB (160 KB in SRAM0  and 32 KB in SRAM1)
     * out of 512 KB of SRAM is alive. All peripherals
     * are turned off and register config is lost.
     * Application should restore the peripheral config
     * after exit form PM3. This consumes lower power
     * than in PM2. A wakeup can happen by timeout (RTC based)
     * or by asserting the WAKEUP 0/1 lines.
     */
    PM3,

    /** (Shutoff Mode): This simulates a shutdown condition.
     * A wakeup can happen by timeout (RTC based) or by
     * asserting the WAKEUP 0/1 lines.
     * This is the lowest power state of MCU.
     * On wakeup execution begins from bootrom as
     * if a fresh bootup has occurred.
     */
    PM4
} power_state_t;

typedef struct
{
    OSStatus (*MicoAdcInitialize)( mico_adc_t adc, uint32_t sampling_cycle );
    OSStatus (*MicoAdcTakeSample)( mico_adc_t adc, uint16_t* output );
    OSStatus (*MicoAdcTakeSampleStreram)( mico_adc_t adc, void* buffer, uint16_t buffer_length );
    OSStatus (*MicoAdcFinalize)( mico_adc_t adc );
} adc_api_t;

typedef struct
{
    OSStatus (*i2c_init)( mico_i2c_device_t* device );
    OSStatus (*i2c_deinit)( mico_i2c_device_t* device );
    bool (*i2c_probe_device)( mico_i2c_device_t* device, int retries );
    OSStatus (*i2c_build_tx_msg)( mico_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length,
                                  uint16_t retries );
    OSStatus (*i2c_build_rx_msg)( mico_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length,
                                  uint16_t retries );
    OSStatus (*i2c_build_combined_msg)( mico_i2c_message_t* message, const void* tx_buffer, void* rx_buffer,
                                        uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries );
    OSStatus (*i2c_transfer)( mico_i2c_device_t* device, mico_i2c_message_t* messages, uint16_t number_of_messages );
} i2c_api_t;

typedef struct
{
    OSStatus (*spi_init)( const mico_spi_device_t* spi );
    OSStatus (*spi_transfer)( const mico_spi_device_t* spi, const mico_spi_message_segment_t* segments,
                              uint16_t number_of_segments );
    OSStatus (*spi_finalize)( const mico_spi_device_t* spi );
} spi_api_t;

typedef struct {
    OSStatus (*iis_init)( const mico_iis_device_t* iis );
    OSStatus (*iis_finalize)( const mico_iis_device_t* iis );
	OSStatus (*iis_transfer)( const mico_iis_device_t* iis, const mico_iis_message_segment_t* segments,
                              uint16_t number_of_segments );
    OSStatus (*iis_write)( const mico_iis_device_t* iis, uint8_t *p_buf, uint32_t size );
    OSStatus (*iis_read)( const mico_iis_device_t* iis, uint8_t *p_buf, uint32_t size );
}iis_api_t;

/* API type define */
typedef struct mico_api_struct
{
    char *library_version;

    /* OS Layer*/
    mico_system_config_t* (*system_config_get)( void );
    void (*system_config_set)( mico_system_config_t *cfg );
    void (*mxchipInit)( );
    OSStatus (*mico_rtos_create_thread)( mico_thread_t* thread, uint8_t priority, const char* name,
                                         mico_thread_function_t function, uint32_t stack_size, void* arg );
    OSStatus (*mico_rtos_delete_thread)( mico_thread_t* thread );
    void (*mico_rtos_suspend_thread)( mico_thread_t* thread );
    void (*mico_rtos_suspend_all_thread)( void );
    long (*mico_rtos_resume_all_thread)( void );
    OSStatus (*mico_rtos_thread_join)( mico_thread_t* thread );
    OSStatus (*mico_rtos_thread_force_awake)( mico_thread_t* thread );
    bool (*mico_rtos_is_current_thread)( mico_thread_t* thread );
    void (*mico_thread_sleep)( uint32_t seconds );
    void (*mico_thread_msleep)( uint32_t milliseconds );
    OSStatus (*mico_rtos_init_semaphore)( mico_semaphore_t* semaphore, int count );
    OSStatus (*mico_rtos_set_semaphore)( mico_semaphore_t* semaphore );
    OSStatus (*mico_rtos_get_semaphore)( mico_semaphore_t* semaphore, uint32_t timeout_ms );
    OSStatus (*mico_rtos_deinit_semaphore)( mico_semaphore_t* semaphore );
    OSStatus (*mico_rtos_init_mutex)( mico_mutex_t* mutex );
    OSStatus (*mico_rtos_lock_mutex)( mico_mutex_t* mutex );
    OSStatus (*mico_rtos_unlock_mutex)( mico_mutex_t* mutex );
    OSStatus (*mico_rtos_deinit_mutex)( mico_mutex_t* mutex );
    OSStatus (*mico_rtos_init_queue)( mico_queue_t* queue, const char* name, uint32_t message_size,
                                      uint32_t number_of_messages );
    OSStatus (*mico_rtos_push_to_queue)( mico_queue_t* queue, void* message, uint32_t timeout_ms );
    OSStatus (*mico_rtos_pop_from_queue)( mico_queue_t* queue, void* message, uint32_t timeout_ms );
    OSStatus (*mico_rtos_deinit_queue)( mico_queue_t* queue );
    bool (*mico_rtos_is_queue_empty)( mico_queue_t* queue );
    OSStatus (*mico_rtos_is_queue_full)( mico_queue_t* queue );
    uint32_t (*mico_get_time)( void );
    OSStatus (*mico_init_timer)( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );
    OSStatus (*mico_start_timer)( mico_timer_t* timer );
    OSStatus (*mico_stop_timer)( mico_timer_t* timer );
    OSStatus (*mico_reload_timer)( mico_timer_t* timer );
    OSStatus (*mico_deinit_timer)( mico_timer_t* timer );
    bool (*mico_is_timer_running)( mico_timer_t* timer );
    int (*mico_create_event_fd)( mico_event_t handle );
    int (*mico_delete_event_fd)( int fd );
    int (*SetTimer)( unsigned long ms, void (*psysTimerHandler)( void ) );
    int (*SetTimer_uniq)( unsigned long ms, void (*psysTimerHandler)( void ) );
    int (*UnSetTimer)( void (*psysTimerHandler)( void ) );

    /* memory management*/
    micoMemInfo_t* (*mico_memory_info)( void );
    void* (*malloc)( size_t size ); // malloc
    void* (*realloc)( void* pv, size_t size ); // realloc
    void (*free)( void* pv );     //free
    void* (*calloc)( int a, int b );     // calloc
    void (*heap_insert)( uint8_t *pv, int len );

    /* Socket */
    int (*socket)( int domain, int type, int protocol );
    int (*setsockopt)( int sockfd, int level, int optname, const void *optval, socklen_t optlen );
    int (*getsockopt)( int sockfd, int level, int optname, const void *optval, socklen_t *optlen );
    int (*bind)( int sockfd, const struct sockaddr *addr, socklen_t addrlen );
    int (*connect)( int sockfd, const struct sockaddr *addr, socklen_t addrlen );
    int (*listen)( int sockfd, int backlog );
    int (*accept)( int sockfd, struct sockaddr *addr, socklen_t *addrlen );
    int (*select)( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout );
    ssize_t (*send)( int sockfd, const void *buf, size_t len, int flags );
    int (*write)( int sockfd, void *buf, size_t len );
    ssize_t (*sendto)( int sockfd, const void *buf, size_t len, int flags,
                       const struct sockaddr *dest_addr,
                       socklen_t addrlen );
    ssize_t (*recv)( int sockfd, void *buf, size_t len, int flags );
    int (*read)( int sockfd, void *buf, size_t len );
    ssize_t (*recvfrom)( int sockfd, void *buf, size_t len, int flags,
                         struct sockaddr *src_addr,
                         socklen_t *addrlen );
    int (*close)( int fd );
    uint32_t (*inet_addr)( char *s );
    char* (*inet_ntoa)( char *s, uint32_t x );
    int (*gethostbyname)( const char * name, uint8_t * addr, uint8_t addrLen );
    void (*set_tcp_keepalive)( int inMaxErrNum, int inSeconds );
    void (*get_tcp_keepalive)( int *outMaxErrNum, int *outSeconds );

    /* SSL */
    void (*ssl_set_cert)( const char *_cert_pem, const char *private_key_pem );
    void* (*ssl_connect)( int fd, int calen, char*ca, int *ssl_errno );
    void* (*ssl_accept)( int fd );
    int (*ssl_send)( void* ssl, char *data, int len );
    int (*ssl_recv)( void* ssl, char *data, int len );
    int (*ssl_close)( void* ssl );
    void (*set_ssl_client_version)( int version );

    /*crypto*/
    void (*InitMd5)( md5_context*md5 );
    void (*Md5Update)( md5_context* md5, unsigned char *input, int ilen );
    void (*Md5Final)( md5_context* md5, uint8_t* hash );
    int (*Md5Hash)( const uint8_t* data, uint32_t len, uint8_t* hash );
    void (*AesEncryptDirect)( Aes* aes, uint8_t* out, const uint8_t* in );
    void (*AesDecryptDirect)( Aes* aes, uint8_t* out, const uint8_t* in );
    int (*AesSetKeyDirect)( Aes* aes, const uint8_t* key, uint32_t len,
                            const uint8_t* iv,
                            int dir );
    int (*aes_encrypt)( int sz, const char * key, const char * in, char * out );
    int (*aes_decrypt)( int sz, const char * key, const char * in, char * out );
    int (*AesSetKey)( Aes* aes, const uint8_t* key, uint32_t len,
                      const uint8_t* iv,
                      int dir );
    int (*AesSetIV)( Aes* aes, const uint8_t* iv );
    int (*AesCbcEncrypt)( Aes* aes, uint8_t* out,
                          const uint8_t* in,
                          uint32_t sz );
    int (*AesCbcDecrypt)( Aes* aes, uint8_t* out,
                          const uint8_t* in,
                          uint32_t sz );

    /* WIFI MGR */
    int (*wlan_get_mac_address)( unsigned char *dest );
    int (*wlan_driver_version)( char* version, int length );
    OSStatus (*micoWlanStart)( network_InitTypeDef_st* inNetworkInitPara );
    OSStatus (*micoWlanStartAdv)( network_InitTypeDef_adv_st* inNetworkInitParaAdv );
    OSStatus (*micoWlanGetIPStatus)( IPStatusTypedef *outNetpara, WiFi_Interface inInterface );
    OSStatus (*micoWlanGetLinkStatus)( LinkStatusTypeDef *outStatus );
    OSStatus (*micoWlanStartScan)( void );
    OSStatus (*micoWlanStartScanAdv)( void );
    OSStatus (*micoWlanPowerOff)( void );
    OSStatus (*micoWlanPowerOn)( void );
    OSStatus (*micoWlanSuspend)( void );
    OSStatus (*micoWlanSuspendStation)( void );
    OSStatus (*micoWlanSuspendSoftAP)( void );
    OSStatus (*micoWlanStartEasyLink)( int inTimeout );
    OSStatus (*micoWlanStartEasyLinkPlus)( int inTimeout );
    OSStatus (*micoWlanStopEasyLink)( void );
    OSStatus (*micoWlanStopEasyLinkPlus)( void );
    OSStatus (*micoWlanStartWPS)( int inTimeout );
    OSStatus (*micoWlanStopWPS)( void );
    OSStatus (*micoWlanStartAirkiss)( int inTimeout );
    OSStatus (*micoWlanStopAirkiss)( void );
    void (*micoWlanEnablePowerSave)( void );
    void (*micoWlanDisablePowerSave)( void );
    void (*wifimgr_debug_enable)( bool enable );
    int (*mico_wlan_monitor_rx_type)( int type );
    int (*mico_wlan_start_monitor)( void );
    int (*mico_wlan_stop_monitor)( void );
    int (*mico_wlan_set_channel)( int channel );
    void (*mico_wlan_register_monitor_cb)( monitor_cb_t fn );
    void (*wlan_set_channel)( int channel );
    int (*mxchip_active_scan)( char*ssid, int is_adv );

    /* CLI APIs */
    void (*wifistate_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*wifidebug_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*wifiscan_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*ifconfig_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*arp_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*ping_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*dns_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*task_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*socket_show_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*memory_show_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*memory_dump_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*memory_set_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*memp_dump_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*driver_state_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
    void (*iperf_Command)( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );

    /* HAL: GPIO; FLASH; UART */
    mico_logic_partition_t* (*MicoFlashGetInfo)( mico_partition_t inPartition );
    OSStatus (*MicoFlashErase)( mico_partition_t inPartition, uint32_t off_set, uint32_t size );
    OSStatus (*MicoFlashWrite)( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer,
                                uint32_t inBufferLength );
    OSStatus (*MicoFlashRead)( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer,
                               uint32_t inBufferLength );
    OSStatus (*MicoFlashEnableSecurity)( mico_partition_t partition, uint32_t off_set, uint32_t size );

    OSStatus (*MicoGpioInitialize)( mico_gpio_t gpio, mico_gpio_config_t configuration );
    OSStatus (*MicoGpioFinalize)( mico_gpio_t gpio );
    OSStatus (*MicoGpioOutputHigh)( mico_gpio_t gpio );
    OSStatus (*MicoGpioOutputLow)( mico_gpio_t gpio );
    OSStatus (*MicoGpioOutputTrigger)( mico_gpio_t gpio );
    bool (*MicoGpioInputGet)( mico_gpio_t gpio );
    OSStatus (*MicoGpioEnableIRQ)( mico_gpio_t gpio, mico_gpio_irq_trigger_t trigger, mico_gpio_irq_handler_t handler,
                                   void* arg );
    OSStatus (*MicoGpioDisableIRQ)( mico_gpio_t gpio );

    OSStatus (*MicoUartInitialize)( mico_uart_t uart, const mico_uart_config_t* config,
                                    ring_buffer_t* optional_rx_buffer );
    OSStatus (*MicoUartFinalize)( mico_uart_t uart );
    OSStatus (*MicoUartSend)( mico_uart_t uart, const void* data, uint32_t size );
    OSStatus (*MicoUartRecv)( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout );
    uint32_t (*MicoUartGetLengthInBuffer)( mico_uart_t uart );
    void (*MicoUartPinRedirect)( mico_uart_t uart );

    /* Power management*/
    int (*pm_mcu_state)( power_state_t state, uint32_t time_dur );
    int (*pm_wakeup_source)( );
    void (*wifi_off_mcu_standby)( int seconds );

    /* uitls */
    int (*debug_putchar)( char *ch );
    void (*MicoSystemReboot)( void );

    /* ALI APIs */
    char* (*get_ali_key)( void );
    char* (*get_ali_secret)( void );

    /* RTC */
    void (*MicoRtcInitialize)( void );
    OSStatus (*MicoRtcGetTime)( mico_rtc_time_t* time );
    OSStatus (*MicoRtcSetTime)( mico_rtc_time_t* time );
    struct tm* (*localtime)( const time_t * time );
    char * (*asctime)( const struct tm *tm );

    int (*wifi_set_country)( int country );
    int (*switch_active_firmrware)( void );
    int (*last_reset_reason)( void );
    int (*aon_write)( uint32_t offset, uint8_t* in, uint32_t len );
    int (*aon_read)( uint32_t offset, uint8_t* out, uint32_t len );

    /* LwIP */
    lwip_api_t *lwip_apis;

    /* FreeRTOS */

    /* PWM */
    pwm_api_t *pwm_apis;

    /* WDG */
    wdg_api_t *wdg_apis;

    int (*ssl_get_fd)( const void* ssl );

    void (*get_random_sequence)( unsigned char *buf, unsigned int size );
    adc_api_t *adc_apis;
    i2c_api_t *i2c_apis;
    spi_api_t *spi_apis;

    int (*ssl_set_loggingcb)( ssl_Logging_cb f );
    int (*wlan_inject_frame)( const uint8_t *buff, size_t len );
    int (*wlan_rx_mgmt_indication)( const enum wlan_bss_type bss_type,
                                    const uint32_t mgmt_subtype_mask,
                                    void (*rx_mgmt_callback)( const enum wlan_bss_type
                                                              bss_type,
                                                              const uint8_t *frame,
                                                              const uint16_t len ) );
    int (*wlan_remain_on_channel)( const bool status, const uint8_t channel,
                                   const uint32_t duration );

    int (*wifi_bridge_mode_enable)( bool hidden_ssid );
    int (*wifi_bridge_mode_disable)( void );

    int (*send_easylink_minus)( uint32_t ip, char *ssid, char *key );

    int (*ssl_socket)( void* ssl );
	int (*mico_wlan_get_channel)(void);
	
	int (*ssl_pending)(void* ssl);
	int (*ssl_get_error)(void* ssl, int ret);
	void (*ssl_set_using_nonblock)(void* ssl, int nonblock);
	void* (*ssl_nonblock_connect)(int fd, int calen, char*ca, int *errno, int timeout);

	char *(*sethostname)( char *name ); // set device name in dhcp table
	int (*mico_wlan_monitor_no_easylink)(void);
	void (*ssl_set_client_cert)(const char *cert_pem, const char *private_key_pem);
	void* (*ssl_connect_sni)(int fd, int calen, char*ca, char *sni_servername, int *errno);

	OSStatus (*micoWlanStartEnt)(network_Enterprise_st* inNetworkInitPara);

    iis_api_t *iis_apis;
} mico_api_t;

typedef struct user_api_struct
{
    uint32_t len;
    uint16_t reserved;
    uint16_t crc16;
    uint32_t magic_num;
    uint32_t app_stack_size;
    uint32_t interface_version;
    char * version;
    char * user_app_version;
    char * PID;
    char * SN;
    mico_uart_t debug_uart;
    int debug_baudrate;

    void (*user_app_in)( const mico_api_t *lib_api_t );
    void (*init_platform)( void );
    int (*application_start)( void );

    /* callback functions */
    void (*ApListCallback)( ScanResult *pApList );
    void (*ApListAdvCallback)( ScanResult_adv *pApAdvList );
    void (*WifiStatusHandler)( WiFiEvent status );
    void (*connected_ap_info)( apinfo_adv_t *ap_info, char *key, int key_len );
    void (*NetCallback)( IPStatusTypedef *pnet );
    void (*RptConfigmodeRslt)( network_InitTypeDef_st *nwkpara );
    void (*easylink_user_data_result)( int datalen, char*data );
    void (*socket_connected)( int fd );
    void (*dns_ip_set)( uint8_t *hostname, uint32_t ip );
    void (*join_fail)( OSStatus err );
    void (*wifi_reboot_event)( void );
    void (*mico_rtos_stack_overflow)( char *taskname );
	uint32_t bootloader_ignore;

  void (*probe_request_rx_cb)(uint8_t *header, int length); // for ouput
  void (*report_ap)(char*ssid, uint8_t *header, int len);
} user_api_t;

#endif
