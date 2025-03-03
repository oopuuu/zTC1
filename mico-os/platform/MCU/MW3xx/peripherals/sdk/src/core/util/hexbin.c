/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wmstdio.h>
#include <wmlog.h>

unsigned int hex2bin(const uint8_t *ibuf, uint8_t *obuf,
		     unsigned max_olen)
{
	unsigned int i;		/* loop iteration variable */
	unsigned int j = 0;	/* current character */
	unsigned int by = 0;	/* byte value for conversion */
	unsigned char ch;	/* current character */
	unsigned int len = strlen((char *)ibuf);
	/* process the list of characters */
	for (i = 0; i < len; i++) {
		if (i == (2 * max_olen)) {
			wmlog("hexbin", "Destination full. "
				"Truncating to avoid overflow.\r\n");
			return j + 1;
		}
		ch = toupper(*ibuf++);	/* get next uppercase character */

		/* do the conversion */
		if (ch >= '0' && ch <= '9')
			by = (by << 4) + ch - '0';
		else if (ch >= 'A' && ch <= 'F')
			by = (by << 4) + ch - 'A' + 10;
		else {		/* error if not hexadecimal */
			return 0;
		}

		/* store a byte for each pair of hexadecimal digits */
		if (i & 1) {
			j = ((i + 1) / 2) - 1;
			obuf[j] = by & 0xff;
		}
	}
	return j + 1;
}

void bin2hex(uint8_t *src, char *dest, unsigned int src_len,
	     unsigned int dest_len)
{
	int i;
	for (i = 0; i < src_len; i++) {
		if (snprintf(dest, dest_len, "%02x", src[i]) >= dest_len) {
			wmlog("hexbin", "Destination full. "
				"Truncating to avoid overflow.\r\n");
			return;
		}
		dest_len -= 2;
		dest += 2;
	}
}
