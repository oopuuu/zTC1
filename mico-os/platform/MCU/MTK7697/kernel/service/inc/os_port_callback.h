/**
 * @file os_port_callback.h
 *
 *  HW dependent callback functions for OS
 *
 */

#ifndef __OS_PORT_CALLBACK_H__
#define __OS_PORT_CALLBACK_H__

#ifndef NO_PORT_CALLBACK

/**
 * @ingroup OS
 * @addtogroup PORT_CALLBACK
 * @{
 */


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

void vConfigureTimerForRunTimeStats( void );
uint32_t ulGetRunTimeCounterValue( void );

#ifdef __cplusplus
}
#endif


/** }@ */

#endif

#endif /* __OS_PORT_CALLBACK_H__ */
