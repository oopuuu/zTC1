enum {
	API_VERSION_V1 = 1,
	API_VERSION_MAX,
};

typedef void* mico_event;
typedef void (*ssl_log_cb)(const int logLevel, const char *const logMessage);
typedef struct Md5 {
    uint32_t  buffLen;   /* length in bytes          */
    uint32_t  loLen;     /* length in bytes   */
    uint32_t  hiLen;     /* length in bytes   */
    uint32_t  buffer[MD5_BLOCK_SIZE  / sizeof(uint32_t)];
    uint32_t  digest[MD5_DIGEST_SIZE / sizeof(uint32_t)];
} Md5;


typedef void (*mgnt_handler_t)(char *buf, int buf_len);

typedef struct {
	/* OS Layer*/
	int (*system_config)(int type, void *value);/* system configuration */
	int (*mxchipInit)();
	OSStatus (*mico_rtos_create_thread)( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, uint32_t stack_size, uint32_t arg );
	OSStatus (*mico_rtos_delete_thread)( mico_thread_t* thread );
	void (*mico_rtos_suspend_thread)(mico_thread_t* thread);
	void (*mico_rtos_suspend_all_thread)(void);
	long (*mico_rtos_resume_all_thread)(void);
	OSStatus (*mico_rtos_thread_join)( mico_thread_t* thread );
	OSStatus (*mico_rtos_thread_force_awake)( mico_thread_t* thread );
	bool (*mico_rtos_is_current_thread)( mico_thread_t* thread );
	void (*mico_thread_sleep)(uint32_t seconds);
	void (*mico_thread_msleep)(uint32_t milliseconds);
	OSStatus (*mico_rtos_init_semaphore)( mico_semaphore_t* semaphore, int count );
	OSStatus (*mico_rtos_set_semaphore)( mico_semaphore_t* semaphore );
	OSStatus (*mico_rtos_get_semaphore)( mico_semaphore_t* semaphore, uint32_t timeout_ms );
	OSStatus (*mico_rtos_deinit_semaphore)( mico_semaphore_t* semaphore );
	OSStatus (*mico_rtos_init_mutex)( mico_mutex_t* mutex );
	OSStatus (*mico_rtos_lock_mutex)( mico_mutex_t* mutex );
	OSStatus (*mico_rtos_unlock_mutex)( mico_mutex_t* mutex );
	OSStatus (*mico_rtos_deinit_mutex)( mico_mutex_t* mutex );
	OSStatus (*mico_rtos_init_queue)( mico_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );
	OSStatus (*mico_rtos_push_to_queue)( mico_queue_t* queue, void* message, uint32_t timeout_ms );
	OSStatus (*mico_rtos_pop_from_queue)( mico_queue_t* queue, void* message, uint32_t timeout_ms );
	OSStatus (*mico_rtos_deinit_queue)( mico_queue_t* queue );
	bool (*mico_rtos_is_queue_empty)( mico_queue_t* queue );
	bool (*mico_rtos_is_queue_full)( mico_queue_t* queue );
	uint32_t (*mico_get_time)(void);
	OSStatus (*mico_init_timer)( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );
	OSStatus (*mico_start_timer)( mico_timer_t* timer );
	OSStatus (*mico_stop_timer)( mico_timer_t* timer );
	OSStatus (*mico_reload_timer)( mico_timer_t* timer );
	OSStatus (*mico_deinit_timer)( mico_timer_t* timer );
	bool (*mico_is_timer_running)( mico_timer_t* timer );
	int (*mico_create_event_fd)(mico_event handle);
	int (*mico_delete_event_fd)(int fd);

	/* memory management*/
	struct mxchip_mallinfo* (*mico_memory_info)(void);
	void* (*malloc)(size_t size); // malloc
	void* (*realloc)(void* pv, size_t size); // realloc
	void (*free)(void* pv);     //free
	void* (*calloc)(size_t nmemb, size_t size);     // calloc
	void (*heap_insert)(uint8_t *pv, int len);

	void (*get_random_sequence)(unsigned char *buf, unsigned int size);
	int (*last_reset_reason)(void);
	int (*aon_write)( uint32_t offset, uint8_t* in ,uint32_t len);
	int (*aon_read )( uint32_t offset, uint8_t* out, uint32_t len);

	/* uitls */
	int (*debug_putchar)(char *ch, int len);
	int (*debug_getchar)(char *ch);
	void (*MicoSystemReboot)( void );

	struct tm* (*localtime)(const time_t * time);
	char * (*asctime)(const struct tm *tm);
} os_api_v1_t;

typedef struct {
	/* SSL */
	void (*ssl_set_cert)(const char *_cert_pem, const char *private_key_pem);
	void* (*ssl_connect)(int fd, int calen, char*ca, int *errno); 
	void* (*ssl_accept)(int fd); 
	int (*ssl_send)(void* ssl, char *data, int len);
	int (*ssl_recv)(void* ssl, char *data, int len);
	int (*ssl_close)(void* ssl);
	void (*set_ssl_client_version)(int version);

	int (*ssl_pending)(void* ssl);
	int (*ssl_get_error)(void* ssl, int ret);
	void (*ssl_set_using_nonblock)(void* ssl, int nonblock);
	int (*ssl_get_fd)(const void* ssl);
	int (*ssl_loggingcb)(ssl_log_cb f);
	
	/*crypto*/
	void (*InitMd5)(Md5*md5);
	void (*Md5Update)(Md5* md5, const uint8_t* data, uint32_t len);
	void (*Md5Final)(Md5* md5, uint8_t* hash);
	int (*Md5Hash)(const uint8_t* data, uint32_t len, uint8_t* hash);
	void (*AesEncryptDirect)(Aes* aes, uint8_t* out, const uint8_t* in);
	void (*AesDecryptDirect)(Aes* aes, uint8_t* out, const uint8_t* in);
	int (*AesSetKeyDirect)(Aes* aes, const uint8_t* key, uint32_t len,
                                const uint8_t* iv, int dir);
	int (*aes_encrypt)(int sz, const char * key, const char * in, char * out);
	int (*aes_decrypt)(int sz, const char * key, const char * in, char * out);
	int  (*AesSetKey)(Aes* aes, const uint8_t* key, uint32_t len,
                              const uint8_t* iv, int dir);
	int  (*AesSetIV)(Aes* aes, const uint8_t* iv);
	int  (*AesCbcEncrypt)(Aes* aes, uint8_t* out,
                                  const uint8_t* in, uint32_t sz);
	int  (*AesCbcDecrypt)(Aes* aes, uint8_t* out,
                                  const uint8_t* in, uint32_t sz);
	void* (*ssl_nonblock_connect)(int fd, int calen, char*ca, int *errno, int timeout);
	void (*ssl_set_client_cert)(const char *_cert_pem, const char *private_key_pem);
	void* (*ssl_connect_sni)(int fd, int calen, char*ca, char *sni_servername, int *errno);
    void (*ssl_set_ecc)(int enable);
} ssl_crypto_api_v1_t;

typedef struct {
	/* WIFI MGR */
	int (*wlan_get_mac_address)(unsigned char *dest);
	int (*wlan_get_mac_address_by_interface)(wlan_if_t wlan_if, unsigned char *dest);
	int (*wlan_driver_version)( char* version, int length );
	OSStatus (*micoWlanStart)(network_InitTypeDef_st* inNetworkInitPara);
	OSStatus (*micoWlanStartAdv)(network_InitTypeDef_adv_st* inNetworkInitParaAdv);
	OSStatus (*micoWlanGetIPStatus)(IPStatusTypedef *outNetpara, WiFi_Interface inInterface);
	OSStatus (*micoWlanGetLinkStatus)(LinkStatusTypeDef *outStatus);
	void (*micoWlanStartScan)(void);
	void (*micoWlanStartScanAdv)(void);
	OSStatus (*micoWlanPowerOff)(void);
	OSStatus (*micoWlanPowerOn)(void);
	OSStatus (*micoWlanSuspend)(void);
	OSStatus (*micoWlanSuspendStation)(void);
	OSStatus (*micoWlanSuspendSoftAP)(void);
	OSStatus (*micoWlanStartEasyLink)(int inTimeout);
	OSStatus (*micoWlanStopEasyLink)(void);
	void (*micoWlanEnablePowerSave)(void);
	void (*micoWlanDisablePowerSave)(void); 
	void (*wifimgr_debug_enable)(bool enable);
	int (*mico_wlan_monitor_rx_type)(int type);
	int (*mico_wlan_start_monitor)(void);
	int (*mico_wlan_stop_monitor)(void);
	int (*mico_wlan_set_channel)(int channel);
	void (*mico_wlan_register_monitor_cb)(monitor_cb_t fn);
	void (*wlan_set_channel)(int channel);
	int (*mxchip_active_scan)(char*ssid, int is_adv);
	int (*send_easylink_minus)(uint32_t ip, char *ssid, char *key)	;
	int (*mico_wlan_get_channel)(void);
    OSStatus (*wifi_manage_custom_ie_add)(wlan_if_t wlan_if, uint8_t *custom_ie, uint32_t len);
    OSStatus (*wifi_manage_custom_ie_delete)(wlan_if_t wlan_if);
    int (*wlan_inject_frame)(const uint8_t *buff, size_t len);
	int (*mico_wlan_monitor_no_easylink)(void);
	int (*wifi_set_country)(int country_code);
	int (*wlan_rx_mgnt_set)(int enable, mgnt_handler_t cb);
	void (*autoconfig_start)(int seconds, int mode);
    void (*wlan_set_softap_tdma)(int value);
    int (*wifi_off_fastly)(void);
    int (*OpenEasylink_softap)(int timeout, char *ssid, char*key, int channel);
} wifi_api_v1_t;

typedef struct {
	/* CLI APIs */
	int (*cli_init)(void);
	int (*cli_register_command)(const struct cli_command *command);
	int (*cli_unregister_command)(const struct cli_command *command);
	void (*wifistate_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*wifidebug_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*wifiscan_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*ifconfig_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*arp_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*ping_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*dns_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*task_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*socket_show_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*memory_show_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*memory_dump_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*memory_set_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*memp_dump_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*driver_state_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
	void (*iperf_Command)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
} cli_api_v1_t;


typedef struct {
	mico_logic_partition_t* (*MicoFlashGetInfo)( mico_partition_t inPartition );
	OSStatus (*MicoFlashErase)(mico_partition_t inPartition, uint32_t off_set, uint32_t size);
	OSStatus (*MicoFlashWrite)( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength);
	OSStatus (*MicoFlashRead)( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer, uint32_t inBufferLength);
	OSStatus (*MicoFlashEnableSecurity)( mico_partition_t partition, uint32_t off_set, uint32_t size );
} flash_api_t;

typedef struct {
	OSStatus (*MicoGpioInitialize)( mico_gpio_t gpio, mico_gpio_config_t configuration );
	OSStatus (*MicoGpioFinalize)( mico_gpio_t gpio );
	OSStatus (*MicoGpioOutputHigh)( mico_gpio_t gpio );
	OSStatus (*MicoGpioOutputLow)( mico_gpio_t gpio );
	OSStatus (*MicoGpioOutputTrigger)( mico_gpio_t gpio );
	bool (*MicoGpioInputGet)( mico_gpio_t gpio );
	OSStatus (*MicoGpioEnableIRQ)( mico_gpio_t gpio, mico_gpio_irq_trigger_t trigger, mico_gpio_irq_handler_t handler, void* arg );
	OSStatus (*MicoGpioDisableIRQ)( mico_gpio_t gpio );
} gpio_api_t;

typedef struct {
	OSStatus (*MicoUartInitialize)( mico_uart_t uart, const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer );
	OSStatus (*MicoUartFinalize)( mico_uart_t uart );
	OSStatus (*MicoUartSend)( mico_uart_t uart, const void* data, uint32_t size );
	OSStatus (*MicoUartRecv)( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout );
	uint32_t (*MicoUartGetLengthInBuffer)( mico_uart_t uart ); 
	void     (*MicoUartPinRedirect)(mico_uart_t uart);
    int (*disable_log_uart)(void);
} uart_api_t;

typedef struct {
	void (*MicoRtcInitialize)(void);
	OSStatus (*MicoRtcGetTime)(mico_rtc_time_t *time);
	OSStatus (*MicoRtcSetTime)(mico_rtc_time_t *time);
} rtc_api_t;

typedef struct {
	/* Power management*/
	int (*pm_mcu_state)(power_state_t state, uint32_t time_dur);
	int (*pm_wakeup_source)(uint8_t wake_source);
	void (*wifi_off_mcu_standby)(uint32_t seconds);
	void (*MicoMcuPowerSaveConfig)( int enable );
} power_save_api_t;

typedef os_api_v1_t os_api_t;
typedef ssl_crypto_api_v1_t ssl_crypto_api_t;
typedef wifi_api_v1_t wifi_api_t;
typedef cli_api_v1_t cli_api_t;

/* API type define */
typedef struct 
{
	os_api_t *os_apis;
	lwip_api_t *lwip_apis;
	ssl_crypto_api_t *ssl_crypto_apis;
	wifi_api_t *wifi_apis;
	cli_api_t *cli_apis;

    flash_api_t *flash_apis;
	gpio_api_t *gpio_apis;
	uart_api_t *uart_apis;
	i2c_api_t *i2c_apis;
	spi_api_t *spi_apis;
	pwm_api_t *pwm_apis;
	rtc_api_t *rtc_apis;
	wdg_api_t *wdg_apis;
	adc_api_t *adc_apis;
	power_save_api_t *ps_apis;
	gtimer_api_t *gtimer_apis;
} kernel_api_v1_t;

typedef kernel_api_v1_t kernel_api_t;

typedef struct new_mico_api_struct
{
	char *library_version;

	int (*mico_api_get)(int version, void *kernel_apis);
} new_mico_api_t;

mico_api_t *moc_adapter(new_mico_api_t *new_mico_api);
