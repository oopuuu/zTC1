/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/** mdev_test.c: Functions for CLI based testing of the mdev interface.
 */
#include <string.h>
#include <stdlib.h>

#include <mdev.h>
#include <wm_os.h>
#include <cli.h>
#include <cli_utils.h>

static mdev_t mdev_devNull __attribute__((section(".extsram.bss")));
static mdev_t mdev_dev1 __attribute__((section(".extsram.bss")));
static mdev_t mdev_dev2 __attribute__((section(".extsram.bss")));
static char *dev1 = "dev1";
static char *dev2 = "dev2";
#define CHECK(ret, gflag, exp_gflag) {\
		if (ret == 0 && gflag == exp_gflag)	\
			wmprintf("Success");	\
		else					\
			wmprintf("Error");	\
	}
#define MAX_MDEV_CMD 4
#define MDEV_READ    2
#define MDEV_WRITE   3
#define MDEV_OPEN    4
#define MDEV_CLOSE   5
#define MDEV_REG     6
#define MDEV_DEREG   7
#define MDEV_IOCTL   8
#define MDEV_ISR     9

mdev_t *dev_test;
int gflag;
UINT32 dev_open(mdev_t *dev, UINT32 flags)
{
	gflag = MDEV_OPEN;
	return 0;
}
UINT32 dev_close(mdev_t *dev)
{
	gflag = MDEV_CLOSE;
	return 0;
}
UINT32 dev_read(struct _mdev_t *dev, UINT8 *buf, UINT32 nbytes, UINT32 flags)
{
	gflag = MDEV_READ;
	return 0;
}
UINT32 dev_write(struct _mdev_t *dev, UINT8 *buf, UINT32 nbytes, UINT32 flags)
{
	gflag = MDEV_WRITE;
	return 0;
}
UINT32 dev_ioctl(mdev_t *dev, mdev_IOCTL_e cmd, void *arg)
{
	gflag = MDEV_IOCTL;
	return 0;
}

int dev_init(mdev_t *dev, char *name)
{
	dev->name = name;
	dev->open = dev_open;
	dev->close = dev_close;
	dev->reset = NULL;
	dev->read = dev_read;
	dev->write = dev_write;
	dev->ioctl = dev_ioctl;
	dev->isr = NULL;
	return 0;
}

void test_reg(int argc, char **argv)
{
	int ret;
	gflag = MDEV_REG;
	if (strcmp(argv[1], "dev1") == 0) {
		ret = dev_init(&mdev_dev1, dev1);
		ret = mdev_register(&mdev_dev1);
		CHECK(ret, gflag, MDEV_REG);
	} else if (strcmp(argv[1], "dev2") == 0) {
		ret = dev_init(&mdev_dev2, dev2);
		ret = mdev_register(&mdev_dev2);
		CHECK(ret, gflag, MDEV_REG);
	} else if (strcmp(argv[1], "devNull") == 0) {
		ret = dev_init(&mdev_devNull, NULL);
		ret = mdev_register(&mdev_dev1);
		CHECK(ret, gflag, MDEV_REG);
	}
}

void test_open(int argc, char **argv)
{
	dev_test = mdev_open(argv[1], 0);
	if (dev_test == NULL && gflag != MDEV_OPEN)
		wmprintf("Error");
	else
		wmprintf("Success");
}
void test_close(int argc, char **argv)
{
	int ret;
	if (strcmp(argv[1], "dev1") == 0 || strcmp(argv[1], "dev2") == 0) {
		/*Test to close devices */
		ret = mdev_close(dev_test);
		CHECK(ret, gflag, MDEV_CLOSE);
	} else if (strcmp(argv[1], "") == 0) {
		/*Test to close devices */
		ret = mdev_close(&mdev_devNull);
		CHECK(ret, gflag, MDEV_CLOSE);
	}
}
void test_dereg(int argc, char **argv)
{
	int ret;
	/* Deregistration test Cases */
	gflag = MDEV_DEREG;
	ret = mdev_deregister(argv[1]);
	CHECK(ret, gflag, MDEV_DEREG);
}


struct cli_command mdev_command[MAX_MDEV_CMD] = {
	{"mdev-test-register", "Register Test", test_reg},
	{"mdev-test-open", "Open Test", test_open},
	{"mdev-test-close", "Close Test", test_close},
	{"mdev-test-dereg", "Register Test", test_dereg},
};

int mdev_cli_init(void)
{
	int i;
	for (i = 0; i < MAX_MDEV_CMD; i++)
		if (cli_register_command(&mdev_command[i]))
			return 1;
	return 0;
}
