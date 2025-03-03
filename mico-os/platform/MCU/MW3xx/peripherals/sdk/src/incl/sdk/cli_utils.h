/* 
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __MTF_UTILS_H__
#define __MTF_UTILS_H__

int string_equal(const char *s1, const char *s2);
int get_uint(const char *arg, unsigned int *dest, unsigned int len);
int get_mac(const char *arg, char *dest, char sep);
void print_mac(const char *mac);

/* Simplified non-re-entrant cli_getopt function.  Call this in a while loop to
 * parse argv.  Here are some caveats:
 *
 * Be sure to set cli_optind to 1 before invoking getopt for the first time.
 * Only short options with preceding '-' are supported.
 * Returns '?' when it encounters an unexpected condition or option
 */
extern int cli_optind;
extern char *cli_optarg;
int cli_getopt(int argc, char **argv, const char *fmt);
unsigned int a2hex_or_atoi(char *value);
unsigned int  a2hex(const char *s);
int ISDIGIT(char *x);
int ishexstring(void *hex);

#endif
