/**
 ******************************************************************************
 * @file    httpd_ssi.c
 * @author  QQ DING
 * @version V1.0.0
 * @date    1-September-2015
 * @brief   This file contains routines for the processing of SSI directives
 *          within shtml files.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

/*
 * Description
 *  SSI processor for simple web server
 *  syntax of a script is
 *   %! <func_name> [optional_arg]
 *   %! : <include html filename>
 *   <standard html>
 *
 */


#include <string.h>

#include "httpd.h"

#include "httpd_priv.h"

static int ssi_ready;
static struct httpd_ssi_call *calls[MAX_REGISTERED_SSIS];

/* Register an SSI handler */
int httpd_register_ssi(struct httpd_ssi_call *ssi_call)
{
	int i;

	if (!ssi_ready)
		return -kInProgressErr;

	/* Verify that the ssi is not already registered */
	for (i = 0; i < MAX_REGISTERED_SSIS; i++) {
		if (!calls[i]) {
			continue;
		}
		if (strncmp(calls[i]->name, ssi_call->name,
			    strlen(calls[i]->name)) == 0) {
			return -kInProgressErr;
		}
	}

	/* Find an unoccupied slot in the table */
	for (i = 0; i < MAX_REGISTERED_SSIS && calls[i]; i++)
			;

	/* Check that we're not overrunning the table */
	if (i == MAX_REGISTERED_SSIS)
		return -WM_E_HTTPD_SSI_MAX;

	calls[i] = ssi_call;
	httpd_d("Register ssi %s at %d", ssi_call->name, i);

	return kNoErr;
}

/* Unregister an SSI handler */
void httpd_unregister_ssi(struct httpd_ssi_call *ssi_call)
{
	int i;

	if (!ssi_ready)
		return;

	for (i = 0; i < MAX_REGISTERED_SSIS; i++) {
		if (!calls[i]) {
			continue;
		}
		if (strncmp(calls[i]->name, ssi_call->name,
			    strlen(calls[i]->name)) == 0) {
			calls[i] = NULL;
			httpd_d("Unregister ssi %s at %d", ssi_call->name, i);
			return;
		}
	}

	return;
}

/* The null function is returned if no match is found */
int nullfunction(const httpd_request_t *req, const char *args, int conn,
		 char *scratch)
{
	return kNoErr;
}

/* Find a matching SSI function from the table and return it */
httpd_ssifunction httpd_ssi(char *name)
{
	struct httpd_ssi_call *f;
	int i;

	if (!ssi_ready)
		return nullfunction;

	/* Find the matching name in the table, return the function. */
	for (i = 0; i < MAX_REGISTERED_SSIS; i++) {
		f = calls[i];
		if (f && strncmp(f->name, name, strlen(f->name)) == 0) {
			httpd_d("Found function: %s", (f)->name);
			return f->function;
		}
	}

	httpd_d("Did not find function %s returning nullfunc", name);

	return nullfunction;
}

/** Initialize the SSI handling data structures */
int httpd_ssi_init(void)
{
	memset(calls, 0, sizeof(struct httpd_ssi_call *) * MAX_REGISTERED_SSIS);
	ssi_ready = 1;
	return kNoErr;
}
