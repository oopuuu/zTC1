/*
 * Copyright 2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef __CURVE25519_DONNA_H__
#define __CURVE25519_DONNA_H__
#include <wmtypes.h>

void curve25519_donna(uint8_t *mypublic, const uint8_t *secret,
		const uint8_t *basepoint);

#endif	/* __CURVE25519_DONNA_H__ */
