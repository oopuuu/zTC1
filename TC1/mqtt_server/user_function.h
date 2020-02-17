
#ifndef __USER_FUNCTION_H_
#define __USER_FUNCTION_H_


#include "mico.h"
#include "micokit_ext.h"

void UserSend(int udp_flag, char *s);
void UserFunctionCmdReceived2(int udp_flag, char* pusrdata);
unsigned char StrToHex(char a, char b);


#endif
