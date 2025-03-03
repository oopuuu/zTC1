/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#include <wmstdio.h>
#include <wmlog.h>
#include <wm_utils.h>
#include <critical_error.h>

/* Default action for critical error. Can be overriden by appplications
 * by writing their own function with this same signature.
 */
WEAK void critical_error(int crit_errno, void *data)
{
	ll_log("free memory %d\r\n", xPortGetFreeHeapSize());
	ll_log("Critical error number: %d (caller addr: %x)\r\n",
			crit_errno, __builtin_return_address(0));
	ll_log("Description: %s\r\n",
			critical_error_msg(crit_errno));
	ll_log("Halting processor\r\n");
	while (1)
		;
}

char *critical_error_msg(int crit_errno)
{
	/* We pass errno to critical_error function as a negative value,
	 * hence we set a switch on -errno. */
	switch (-crit_errno) {
	case CRIT_ERR_WLAN_INIT_FAILED:
		return "WLAN initialization failed!";
	case CRIT_ERR_APP_FRAMEWORK_INIT_FAILED:
		return "App Framework init failed!";
	case CRIT_ERR_APP_FRAMEWORK:
		return "App Framework error!";
	case CRIT_ERR_HARD_FAULT:
		return "Hardfault occurred!";
	default:
		return "Other error!";
	}
}
