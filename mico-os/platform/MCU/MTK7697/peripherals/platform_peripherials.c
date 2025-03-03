#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

/**
 * Enable MCU powersave
 *
 * @return @ref OSStatus
 */
OSStatus platform_mcu_powersave_enable( void ){ return kNotPreparedErr; }


/**
 * Disable MCU powersave
 *
 * @return @ref OSStatus
 */
OSStatus platform_mcu_powersave_disable( void ){ return kNotPreparedErr; }

/**
 * Enter standby mode, and wait a period to wakup
 *
 * @param[in] secondsToWakeup : seconds to wakeup
 */
void platform_mcu_enter_standby( uint32_t secondsToWakeup ){ }


/**
 * Notify the software stack that MCU has exited powersave mode due to interrupt
 *
 * @return @ref OSStatus
 */
void platform_mcu_powersave_exit_notify( void ){ }


/**
 * Initialise the specified SPI interface
 *
 * @param[in] spi_interface : SPI interface
 * @param[in] config        : SPI configuratin
 *
 * @return @ref OSStatus
 */
OSStatus platform_wlan_spi_init( const platform_gpio_t* chip_select ){ return kNotPreparedErr; }

/**
 * Transfer data over the specified SPI interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_wlan_spi_transfer( const platform_gpio_t* chip_select, const platform_spi_message_segment_t* segments, uint16_t number_of_segments ){ return kNotPreparedErr; }


/** Initialises a SPI slave interface
 *
 * @param[in]  driver     : the SPI slave driver to be initialised
 * @param[in]  peripheral : the SPI peripheral interface to be initialised
 * @param[in]  config     : SPI slave configuration
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config ){ return kNotPreparedErr; }


/** De-initialises a SPI slave interface
 *
 * @param[in]  driver : the SPI slave driver to be de-initialised
 *
 * @return @ref OSStatus
 */

WEAK OSStatus platform_spi_slave_deinit( platform_spi_slave_driver_t* driver ){ return kNotPreparedErr; }


/** Receive command from the remote SPI master
 *
 * @param[in]   driver      : the SPI slave driver
 * @param[out]  command     : pointer to the variable which will contained the received command
 * @param[in]   timeout_ms  : timeout in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms ){ return kNotPreparedErr; }


/** Transfer data to/from the remote SPI master
 *
 * @param[in]  driver      : the SPI slave driver
 * @param[in]  direction   : transfer direction
 * @param[in]  buffer      : the buffer which contain the data to transfer
 * @param[in]  timeout_ms  : timeout in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms ){ return kNotPreparedErr; }


/** Send an error status over the SPI slave interface
 *
 * @param[in]  driver       : the SPI slave driver
 * @param[in]  error_status : SPI slave error status
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status ){ return kNotPreparedErr; }


/** Generate an interrupt on the SPI slave interface
 *
 * @param[in]  driver            : the SPI slave driver
 * @param[in]  pulse_duration_ms : interrupt pulse duration in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms ){ return kNotPreparedErr; }

/**
 * Initialise UART standard I/O
 *
 * @param[in,out] driver    : UART STDIO driver
 * @param[in]     interface : UART STDIO interface
 * @param[in]     config    : UART STDIO configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_stdio_init ( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config ){ return kNotPreparedErr; }


/**
 * Get current value of nanosecond clock
 *
*/
uint64_t platform_get_nanosecond_clock_value( void ){ return kNotPreparedErr; }


/**
 * Deinitialize nanosecond clock
 *
 */
void platform_deinit_nanosecond_clock( void ){ }


/**
 * Reset nanosecond clock
 *
 */
void platform_reset_nanosecond_clock( void ){ }


/**
 * Initialize nanosecond clock
 *
 */
void platform_init_nanosecond_clock( void ){ }

/**
 * Nanosecond delay
 *
 */
void platform_nanosecond_delay( uint64_t delayns ){ }
