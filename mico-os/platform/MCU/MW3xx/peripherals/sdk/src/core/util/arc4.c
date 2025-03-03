/* arc4.c
 *
 * yhb port from wolfssl
 */
#include "stdint.h"

enum {
	ARC4_ENC_TYPE   = 4,    /* cipher unique type */
    ARC4_STATE_SIZE = 256
};

/* ARC4 encryption and decryption */
typedef struct Arc4 {
    uint8_t x;
    uint8_t y;
    uint8_t state[ARC4_STATE_SIZE];
} Arc4;

void Arc4Process(Arc4*, uint8_t*, const uint8_t*, uint32_t);
void Arc4SetKey(Arc4*, const uint8_t*, uint32_t);


void Arc4SetKey(Arc4* arc4, const uint8_t* key, uint32_t length)
{
    uint32_t i;
    uint32_t keyIndex = 0, stateIndex = 0;


    arc4->x = 1;
    arc4->y = 0;

    for (i = 0; i < ARC4_STATE_SIZE; i++)
        arc4->state[i] = (uint8_t)i;

    for (i = 0; i < ARC4_STATE_SIZE; i++) {
        uint32_t a = arc4->state[i];
        stateIndex += key[keyIndex] + a;
        stateIndex &= 0xFF;
        arc4->state[i] = arc4->state[stateIndex];
        arc4->state[stateIndex] = (uint8_t)a;

        if (++keyIndex >= length)
            keyIndex = 0;
    }
}


static uint8_t MakeByte(uint32_t* x, uint32_t* y, uint8_t* s)
{
    uint32_t a = s[*x], b;
    *y = (*y+a) & 0xff;

    b = s[*y];
    s[*x] = (uint8_t)b;
    s[*y] = (uint8_t)a;
    *x = (*x+1) & 0xff;

    return s[(a+b) & 0xff];
}


void Arc4Process(Arc4* arc4, uint8_t* out, const uint8_t* in, uint32_t length)
{
    uint32_t x;
    uint32_t y;


    x = arc4->x;
    y = arc4->y;

    while(length--)
        *out++ = *in++ ^ MakeByte(&x, &y, arc4->state);

    arc4->x = (uint8_t)x;
    arc4->y = (uint8_t)y;
}

void encrypt_arc4(uint8_t *in, uint8_t *out, int len)
{
	Arc4 rc4;
	uint8_t key[] = {0xFB, 0xF2, 0x92, 0xD9, 0x07, 0x93, 0x37, 0xBB};
	
	Arc4SetKey(&rc4, key, sizeof(key));
	Arc4Process(&rc4, out, in, len);
}

