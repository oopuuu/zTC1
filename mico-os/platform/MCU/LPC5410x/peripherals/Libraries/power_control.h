#ifndef _POWER_CONTROL_H
#define _POWER_CONTROL_H

#define MCU_FREQ_UNIT 12000000

#ifdef _cplusplus
extern "C" {
#endif

typedef enum TASK_TYPE {
	IDLE_TASK = 0,		// 12
	MICO_TASK = 2,		// 84
} TASK_T;

typedef struct _FRQ_CTRL_T {
	uint32_t currentFreq;
	TASK_T currentTask;
} Freq_CTRL_T;

void PwrCtlLowPowerMode(unsigned int powerMode);

unsigned int PwrCtlSetMcuFrequency(unsigned int freq);

unsigned int TaskProcNotify(TASK_T task, uint32_t isOn);

unsigned int TaskSetMcuFrequency(unsigned int freq, TASK_T task);

void RTCSetUp(void);

#if 0
void vApplicationSleep(unsigned short xExpectedIdleTime);
#endif
void disable_interrupts(void);

void enable_interrupts(void);

#ifdef _cplusplus
}
#endif

#endif

