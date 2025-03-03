/**
 ******************************************************************************
 * @file    SocketUtils.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file contains function called by socket operation
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

#include "SocketUtils.h"
#include "mico.h"

#define socket_utils_log(M, ...) custom_log("SocketUtils", M, ##__VA_ARGS__)
#define socket_utils_log_trace() custom_log_trace("SocketUtils")

OSStatus SocketSend( int fd, const uint8_t *inBuf, size_t inBufLen )
{
    socket_utils_log_trace();
    OSStatus err = kParamErr;
    ssize_t writeResult;
    int selectResult;
    size_t numWritten;
    fd_set writeSet;
    struct timeval t;

    require( fd>=0, exit );
    require( inBuf, exit );
    require( inBufLen, exit );

    err = kNotWritableErr;

    FD_ZERO( &writeSet );
    FD_SET( fd, &writeSet );

    t.tv_sec = 5;
    t.tv_usec = 0;
    numWritten = 0;
    do
    {
        selectResult = select( fd + 1, NULL, &writeSet, NULL, &t );
        require( selectResult >= 1, exit );

        writeResult = write( fd, (void *)( inBuf + numWritten ), ( inBufLen - numWritten ) );
        require( writeResult > 0, exit );

        numWritten += writeResult;

        //socket_utils_log("Wrote %zu / %zu bytes", numWritten, inBufLen);

    } while( numWritten < inBufLen );

    require_action( numWritten == inBufLen,
                    exit,
                    socket_utils_log("ERROR: Did not write all the bytes in the buffer. BufLen: %zu, Bytes Written: %zu", inBufLen, numWritten ); err = kUnderrunErr );

    err = kNoErr;

exit:
    return err;
}

void SocketClose(int* fd)
{
    int tempFd = *fd;
    if ( tempFd < 0 )
      return;
    *fd = -1;
    close(tempFd);
}

void SocketCloseForOSEvent(int* fd)
{
    int tempFd = *fd;
    if ( tempFd < 0 )
      return;
    *fd = -1;
    mico_delete_event_fd(tempFd);
}

void SocketAccept(int *plocalTcpClientsPool, int maxClientsNum, int newFd)
{
    int minFd = plocalTcpClientsPool[0];
    int minFdIndex = 0;
    int i;

    for(i = 0; i < maxClientsNum; i++) {
        if(minFd >= plocalTcpClientsPool[i]){
            minFd = plocalTcpClientsPool[i];
            minFdIndex = i;
        }
    }

    if(minFd != -1){
        SocketClose(&minFd);
        socket_utils_log("Force close, %d@%d", minFd, minFdIndex);
    }
          
    plocalTcpClientsPool[minFdIndex] = newFd;  
}

