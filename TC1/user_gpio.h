#ifndef __USER_KEY_H_
#define __USER_KEY_H_

#include "mico.h"
#include "micokit_ext.h"

#define SWITCH_TOTAL_SOCKET 0
#define SWITCH_SOCKET_1 1
#define SWITCH_SOCKET_2 2
#define SWITCH_SOCKET_3 3
#define SWITCH_SOCKET_4 4
#define SWITCH_SOCKET_5 5
#define SWITCH_SOCKET_6 6
#define SWITCH_LED_ENABLE 7

extern char socket_status[32];

void UserLedSet(char x);
void KeyInit(void);
void UserRelaySet(unsigned char x,unsigned char y);
void UserRelaySetAll(char y);
bool RelayOut(void);
char* GetSocketStatus();
char* GetShortClickConfig();
void SetSocketStatus(char* socket_status);

#endif
