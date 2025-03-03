#ifndef __MXCHIP_DEBUG_H__
#define __MXCHIP_DEBUG_H__


typedef int (*debug_printf)( char*msg, ... );

enum {
	SYSTEM_DEBUG_NONE  = 0,
    SYSTEM_DEBUG_ERROR = 1,
    SYSTEM_DEBUG_DEBUG = 2,
    SYSTEM_DEBUG_INFO  = 3,
};

extern debug_printf pPrintffunc;
extern int debug_level;

#if 0
#define system_debug_printf(level, ...) \
do {\
    if ((level <= debug_level) && (pPrintffunc != NULL))\
        pPrintffunc(__VA_ARGS__);\
}while(0)
#else
#include "syslog.h"
#define system_debug_printf(level, ...) \
do {\
    extern uint8_t debug_enable;\
    if(debug_enable){\
        LOG_E(common, __VA_ARGS__);\
    }\
} while(0)
#endif

#define cmd_printf(...) do{\
                                if (xWriteBufferLen > 0) {\
                                    snprintf(pcWriteBuffer, xWriteBufferLen, __VA_ARGS__);\
                                    xWriteBufferLen-=strlen(pcWriteBuffer);\
                                    pcWriteBuffer+=strlen(pcWriteBuffer);\
                                }\
                             }while(0)


void system_debug_enable(int level, debug_printf callback);



#endif
