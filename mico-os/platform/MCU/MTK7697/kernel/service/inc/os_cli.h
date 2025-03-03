/**
 * @file os_cli.h
 *
 *  OS related commands
 *
 */

#ifndef __OS_CLI_H__
#define __OS_CLI_H__

#if defined(MTK_MINICLI_ENABLE)

/**
 * @ingroup OS
 * @addtogroup CLI
 * @{
 */

#include "cli.h"


#ifdef __cplusplus
extern "C" {
#endif


extern cmd_t os_cli[];

#define OS_CLI_ENTRY { "os", "os info", NULL, os_cli },


#ifdef __cplusplus
}
#endif


/** }@ */

#endif /* #if defined(MTK_MINICLI_ENABLE) */

#endif /* __OS_CLI_H__ */
