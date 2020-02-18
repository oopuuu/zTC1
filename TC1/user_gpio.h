#ifndef __USER_KEY_H_
#define __USER_KEY_H_

#include "mico.h"
#include "micokit_ext.h"

extern char socket_status[32];

void UserLedSet(char x);
void KeyInit(void);
void UserRelaySet(unsigned char x,unsigned char y);
void UserRelaySetAll(char y);
bool RelayOut(void);
char* GetSocketStatus();
void SetSocketStatus(char* socket_status);

#endif
