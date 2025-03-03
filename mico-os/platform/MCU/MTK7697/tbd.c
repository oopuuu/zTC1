#include "mico.h"

/*
 * mico.h
 */
//int MicoGetRfVer( char* outVersion, uint8_t inLength )
//{
//	return 1;
//}
//char* MicoGetVer( void )
//{
//	return NULL;
//}
//void MicoInit( void )
//{
//}

/*
 * mico_wlan.h
 */
//OSStatus micoWlanStart(network_InitTypeDef_st* inNetworkInitPara)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStartAdv(network_InitTypeDef_adv_st* inNetworkInitParaAdv)
//{
//	return kNoErr;
//}
//OSStatus micoWlanGetIPStatus(IPStatusTypedef *outNetpara, WiFi_Interface inInterface)
//{
//	return kNoErr;
//}
//OSStatus micoWlanGetLinkStatus(LinkStatusTypeDef *outStatus)
//{
//	return kNoErr;
//}
//void micoWlanStartScan(void)
//{
//}
//void micoWlanStartScanAdv(void)
//{
//}
OSStatus micoWlanPowerOff(void)
{
	return kNoErr;
}
OSStatus micoWlanPowerOn(void)
{
	return kNoErr;
}
//OSStatus micoWlanSuspend(void)
//{
//	return kNoErr;
//}
//OSStatus micoWlanSuspendStation(void)
//{
//	return kNoErr;
//}
//OSStatus micoWlanSuspendSoftAP(void)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStartEasyLink(int inTimeout)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStartEasyLinkPlus(int inTimeout)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStopEasyLink(void)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStopEasyLinkPlus(void)
//{
//	return kNoErr;
//}
OSStatus micoWlanStartWPS(int inTimeout)
{
	return kNoErr;
}
OSStatus micoWlanStopWPS(void)
{
	return kNoErr;
}
//OSStatus micoWlanStartAirkiss(int inTimeout)
//{
//	return kNoErr;
//}
//OSStatus micoWlanStopAirkiss(void)
//{
//	return kNoErr;
//}
void micoWlanEnablePowerSave(void)
{
}
void micoWlanDisablePowerSave(void)
{
}
//void wifimgr_debug_enable(bool enable)
//{
//}
//void wlan_get_mac_address (uint8_t *mac)
//{
//}

/*
 * mico_rtos.h
 */
//int SetTimer(unsigned long ms, void (*psysTimerHandler)(void))
//{
//	return 1;
//}
//int SetTimer_uniq(unsigned long ms, void (*psysTimerHandler)(void))
//{
//	return 1;
//}
//int UnSetTimer(void (*psysTimerHandler)(void))
//{
//	return 1;
//}
//int mico_create_event_fd(mico_event handle)
//{
//	return 1;
//}
//int mico_delete_event_fd(int fd)
//{
//	return 1;
//}
//void event_rx_cb(void *queue)
//{
//}

/*
 * mico_security.h
 */
//void InitMd5(md5_context *ctx)
//{
//}
//void Md5Update(md5_context *ctx, unsigned char *input, int ilen)
//{
//}
//void Md5Final(md5_context *ctx, unsigned char output[16])
//{
//}

/*
 * mfg
 */
//int mfg_connect (char *ssid)
//{
//	return 1;
//}
//int mfg_scan (void)
//{
//	return 1;
//}

/*
 * CLI
 */
//void wifiscan_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void wifistate_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void wifidebug_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void ifconfig_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void arp_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void ping_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void dns_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void socket_show_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void task_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void memory_show_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void memory_dump_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void memory_set_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void memp_dump_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
//void driver_state_Command(char *pcWriteBuffer, int xWriteBufferLen,int argc, char **argv)
//{
//}
