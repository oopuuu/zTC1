/*
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef RTC_H
#define RTC_H

#include <wmtime.h>

#ifdef CONFIG_HW_RTC
#define rtc_init       hwrtc_init
#define rtc_time_set   hwrtc_time_set
#define rtc_time_get   hwrtc_time_get

extern void hwrtc_init(void);
extern int hwrtc_time_set(time_t time);
extern time_t hwrtc_time_get(void);
extern void hwrtc_time_update(void);
#else
/* Use Software RTC (Real Time Clock) if no hardware RTC is available */
#define rtc_init       swrtc_init
#define rtc_time_set   swrtc_time_set
#define rtc_time_get   swrtc_time_get

extern void swrtc_init(void);
extern int swrtc_time_set(time_t time);
extern time_t swrtc_time_get(void);

/** Time stored in POSIX format (Seconds since epoch) */
extern time_t cur_posix_time;

/** Absolute number of ticks */
extern unsigned int abs_tick_count;
#endif
#endif
