/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

/* healthmon_cli.c: CLI for the healthmon
 *
 */
#include <cli.h>
#include <healthmon.h>

#include "healthmon_int.h"

static void cmd_healthmon_stat(int argc, char **argv)
{
	healthmon_display_stat();
}

static struct cli_command healthmon_cmds[] = {
	{"healthmon-stat", "", cmd_healthmon_stat},
};

int healthmon_register_cli(void)
{
	return cli_register_command(&healthmon_cmds[0]);
}
