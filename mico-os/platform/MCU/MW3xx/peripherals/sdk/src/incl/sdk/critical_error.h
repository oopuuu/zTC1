/*! \file critical_error.h
 * \brief Critical Error handler module
 *
 * This is used for handling critical error conditions.
 * The default action can be overridden by applications.
 */
/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef _CRITICAL_ERROR_H_
#define _CRITICAL_ERROR_H_
#include <wmerrno.h>

/** Types of critical errors. A negation of the below values is passed
 * to critical_error() handler.
 */
typedef enum {
	/** WLAN initialization failed. */
	CRIT_ERR_WLAN_INIT_FAILED = MOD_ERROR_START(MOD_CRIT_ERR),
	/** Application Framework initialization failed. */
	CRIT_ERR_APP_FRAMEWORK_INIT_FAILED,
	/** Critical error occurred in Application Framework. */
	CRIT_ERR_APP_FRAMEWORK,
	/** Hard fault occurred	*/
	CRIT_ERR_HARD_FAULT,
	/** Last entry. Applications can define their own error numbers
	 * after this.
	 */
	CRIT_ERR_LAST,
} critical_errno_t;

/** Critical error handler
 *
 * This function handles critical errors. Applications can define
 * their own function with the below name and signature to override
 * the default behavior.
 * Default behaviour is to just print the error message (if console
 * is enabled) and stall.
 *
 * \param[in] crit_errno The error number of type \ref critical_errno_t.
 * Note that a negation of the error number is passed here.
 * \param[in] data Pointer to any data associated with the error.
 * This will be valid only if it is explicitly mentioned in the
 * documentation of the particular error.
 */
void critical_error(int crit_errno, void *data);

/** Get critical error message
 *
 * This is a convenience API that can be used inside a critical error
 * handler. The critical error number in itself is not very user friendly.
 * This API converts in into a user friendly message.
 *
 * \param[in] crit_errno The critical error number. This should be the same
 * value as received by critical_error()
 *
 * \return A user friendly string associated with the critical error.
 */
char *critical_error_msg(int crit_errno);
#endif /* _CRITICAL_ERROR_H_ */
