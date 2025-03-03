/**********************************************************
Date: 		OCT 28, 2006
Project :	TFTP Server
Programer:	Reza Rahmanian, Craig Holmes
File:		Header Files
Purpose:	This file contains all the needed header files 
		for the implementation of the TFTP client and servers.
***********************************************************/

#include <stdio.h>		/*for input and output */
#include <string.h>		/*for string functions */
#include <stdlib.h> 		/**/
#include "mico.h"

enum {
    TFTP_ERR_SOCK,
    TFTP_ERR_,
};

#define ERROR -1

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 0x04
#define ERR 0x05

#define TIMEOUT 2000 /*amount of time to wait for an ACK/Data Packet in 1000microseconds 1000 = 1 second*/
#define RETRIES 3 /* Number of times to resend a data OR ack packet beforing giving up */
#define MAXACKFREQ 1 /* Maximum number of packets before ack */
#define MAXDATASIZE 512 /* Maximum data size allowed */

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */

typedef struct {
    char     filename[32];
    uint32_t filelen;
    uint32_t flashaddr; // the flash address of this file
    int      flashtype; // SPI flash or Internal flash.
} tftp_file_info_t;


int tsend (tftp_file_info_t *fileinfo, uint32_t ipaddr);
/*a function to get a file from the server*/
int tget (tftp_file_info_t *fileinfo, uint32_t ipaddr);

