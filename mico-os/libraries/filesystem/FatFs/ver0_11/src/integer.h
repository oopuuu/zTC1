/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#include <stdint.h>

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>
#ifndef _INC_TCHAR
#define _INC_TCHAR
#endif
#endif

/* This type MUST be 8 bit */
typedef uint8_t	FATFS_BYTE;

/* These types MUST be 16 bit */
typedef uint16_t	FATFS_WORD;
typedef uint16_t	FATFS_WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef unsigned int	FATFS_UINT;

/* These types MUST be 32 bit */
typedef uint32_t	FATFS_DWORD;

#endif
