/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/** mdev.c: Functions for the Marvell Device Driver Interface
 */
#include <string.h>

#include <mdev.h>
#include <wm_os.h>

mdev_t *pMdevList = NULL;

/**
 * registering module
 *
 * @return
 */
uint32_t mdev_register(mdev_t *dev)
{
	mdev_t *pMdev = pMdevList;
	mdev_t *pTargetMdev = dev;
	uint32_t int_sta;

	if (pTargetMdev == NULL || pTargetMdev->name == NULL)
		return 1;

	pTargetMdev->pNextMdev = NULL;

	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, dev->name)) {
			os_exit_critical_section(int_sta);
			return 1;
		}

		if (pMdev->pNextMdev == NULL) {
			pMdev->pNextMdev = pTargetMdev;
			pTargetMdev->pNextMdev = NULL;
			os_exit_critical_section(int_sta);
			return 0;
		}
		pMdev = pMdev->pNextMdev;
	}

	pMdevList = pTargetMdev;

	os_exit_critical_section(int_sta);
	return 0;
}

/**
 * deregistering module
 *
 * @return
 */
uint32_t mdev_deregister(const char *name)
{
	mdev_t *pMdev = pMdevList;
	mdev_t *pPrevMdev = pMdevList;
	uint32_t int_sta;
	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, name)) {
			pPrevMdev->pNextMdev = pMdev->pNextMdev;
			os_exit_critical_section(int_sta);
			return 0;
		}
		pPrevMdev = pMdev;
		pMdev = pMdev->pNextMdev;
	}
	os_exit_critical_section(int_sta);
	return 1;
}

mdev_t *mdev_get_handle(const char *dev_name)
{
	mdev_t *pMdev = pMdevList;
	uint32_t int_sta;
	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, dev_name)) {
			os_exit_critical_section(int_sta);
			return (void *)pMdev;
		}
		pMdev = pMdev->pNextMdev;
	}
	os_exit_critical_section(int_sta);
	return NULL;
}
