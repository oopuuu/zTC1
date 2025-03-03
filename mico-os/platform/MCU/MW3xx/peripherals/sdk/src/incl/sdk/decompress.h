/*
 * Copyright (C) 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef __DECOMPRESS_H__
#define __DECOMPRESS_H__

#include <wmtypes.h>
#include <xz.h>

int xz_uncompress_init(struct xz_buf *stream, uint8_t *sbuf, uint8_t *dbuf);
int xz_uncompress_stream(struct xz_buf *stream, uint8_t *sbuf, uint32_t slen,
		uint8_t *dbuf, uint32_t dlen, uint32_t *decomp_len);
void xz_uncompress_end();

#endif /* __DECOMPRESS_H__ */
