/** @file profiler.h
 *  @brief This file contains definition for function profiling functions.
 *
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef PROFILER_H
#define PROFILER_H

typedef enum prof_commands {
	PROF_CLEAR,
} prof_cmd_t;

int prof_state_update(prof_cmd_t prof_cmd);
int prof_cli_init(void);

#endif /*! PROFILER_H */
