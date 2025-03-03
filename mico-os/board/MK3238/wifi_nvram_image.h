/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

/** @file
 *  NVRAM variables which define BCM43438A0 Parameters for the
 *  MXCHIP EMW3238 module.
 *
 */

#ifndef INCLUDED_NVRAM_IMAGE_H_
#define INCLUDED_NVRAM_IMAGE_H_

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NVRAM_GENERATED_MAC_ADDRESS   "macaddr=C8:93:46:00:00:01"
/**
 * Character array of NVRAM image
 */

static const char wifi_nvram_image[] =

		"manfid=0x2d0"		 												"\x00"
		"prodid=0x0726"														"\x00"
		"vendid=0x14e4"														"\x00"
		"devid=0x43e2"														"\x00"
		"boardtype=0x0726"													"\x00"
		"boardrev=0x1101"													"\x00"
		"boardnum=22"														"\x00"
        NVRAM_GENERATED_MAC_ADDRESS                                         "\x00"
		"sromrev=11"														"\x00"
		"boardflags=0x00404201"												"\x00"
		"xtalfreq=37400"													"\x00"
		"nocrc=1"															"\x00"
		"ag0=255"															"\x00"
		"aa2g=1"															"\x00"
		"ccode=ALL"															"\x00"
		"pa0itssit=0x20"													"\x00"
		"extpagain2g=0"														"\x00"
		"pa2ga0=-189,5883,-691"												"\x00"
		"AvVmid_c0=0x0,0xc8"												"\x00"
		"cckpwroffset0=5"													"\x00"
		"maxp2ga0=72"														"\x00"
		"txpwrbckof=6"														"\x00"
		"cckbw202gpo=0x1111"												"\x00"
		"legofdmbw202gpo=0x66666666"										"\x00"
		"mcsbw202gpo=0x88888888"											"\x00"
		"ofdmdigfilttype=18"												"\x00"
		"ofdmdigfilttypebe=18"												"\x00"
		"papdmode=1"														"\x00"
		"il0macaddr=00:90:4c:c5:12:38"										"\x00"
		"wl0id=0x431b"														"\x00"
		//#OOB parameters
		"hostwake=0x40"														"\x00"
		"hostrdy=0x41"														"\x00"
		"deadman_to=0xffffffff"												"\x00"					
        //#OOB Enable
		"muxenab=0x10"  		                                            "\x00"
        //#CE 1.8.1
        "edonthd=-70"                                                       "\x00"
        "edoffthd=-76"                                                      "\x00"
        "\x00\x00";

#ifdef __cplusplus
} /* extern "C" */
#endif

#else /* ifndef INCLUDED_NVRAM_IMAGE_H_ */

#error Wi-Fi NVRAM image included twice

#endif /* ifndef INCLUDED_NVRAM_IMAGE_H_ */
