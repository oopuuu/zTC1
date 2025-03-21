#ifndef __USER_KEY_H_
#define __USER_KEY_H_

#include "mico.h"
#include "micokit_ext.h"

#define NO_FUNCTION 0x0F
#define SWITCH_ALL_SOCKETS 0
#define SWITCH_SOCKET_1 1
#define SWITCH_SOCKET_2 2
#define SWITCH_SOCKET_3 3
#define SWITCH_SOCKET_4 4
#define SWITCH_SOCKET_5 5
#define SWITCH_SOCKET_6 6
#define SWITCH_LED_ENABLE 7
#define REBOOT_SYSTEM 8
#define CONFIG_WIFI 9
#define RESET_SYSTEM 10

extern char socket_status[32];

void UserLedSet(char x);
void KeyInit(void);
void UserRelaySet(unsigned char x, char y);
void UserRelaySetAll(char y);
bool RelayOut(void);
char* GetSocketStatus();
char* GetButtonClickConfig();
void SetSocketStatus(char* socket_status);
void set_key_map(int index, char short_func, char long_func);
char get_short_func(char val);
char get_long_func(char val);
void StartLedBlink(uint8_t times);

#endif
