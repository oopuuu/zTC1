/**********************************************************
Date: 		NOV 28th, 2006
Project :	TFTP Client
Programers:	
Jonathan Felske
Andrew Fullard 
Craig Holmes
Reza Rahmanian 
Adam Tomalty 
File:		TFTP Client (main)
Purpose:	A TFTP client that will request a connections from
		the server and transefet files.
Notes:		Here we are using the sendto and recvfrom
		functions so the server and client can exchange data.
***********************************************************/

#include "tftp.h"

static const char err_msg [7][40] = {"Not defined, see error message if any",
                        "File not fount",
                        "Access Violation",
                        "Disk full, or allocation exceeded",
                        "Illegal TFTP operation",
                        "Unknown transfer ID",
                        "File already exists"};

/*a function to create the request packet, read or write*/
static int req_packet (int opcode, char *filename, char *mode, unsigned char buf[]);
/*a function to creat an ACK packet*/
//static int ack_packet (int block, char buf[]);
/*a function to create the Error packets*/
static int err_packet (int err_code, const char *err_msg, unsigned char buf[]);

/* A function that will create a request packet*/
/*I'm not sure if I need the last 0x00 because sprintf will end the string with \0*/
static int
req_packet (int opcode, char *filename, char *mode, unsigned char buf[])
{
  int i = 0, len;
  
  buf[i++] = 0x00;
  buf[i++] = (char)opcode;
  len = strlen(filename);
  memcpy(&buf[i], filename, len);
  i+=len;
  buf[i++] = 0x00;
  len = strlen(mode);
  memcpy(&buf[i], mode, len);
  i+=len;
  buf[i++] = 0x00;

  return i;
}



/* A function that will create an ACK packet*/
/*problem that we will have here is that we can only get up to 255 blocks*/
/*
static int
ack_packet (int block, char buf[])
{
  buf[0] = 0x00;
  buf[1] = ACK;
  buf[2] = (block & 0xFF00) >> 8;
  buf[3] = (block & 0x00FF);

  return 4;
}
*/

/* A function that will create an error packet based on the error code*/
static int
err_packet (int err_code, const char *err_msg, unsigned char buf[])
{
  int i = 0;
  int len;
  
  len = strlen(err_msg);
  memset (buf, 0, 5 + len);
  buf[i++] = 0x00;
  buf[i++] = ERR;
  buf[i++] = 0x00;
  buf[i++] = err_code;
  memcpy(&buf[i], err_msg, len);
  i+=len;
  buf[i++] = 0x00;

  return i;
}


/*
*This function is called when the client would like to upload a file to the server.
*/
int tsend (tftp_file_info_t *fileinfo, uint32_t ipaddr)
{
  int len, server_len, opcode= WRQ, ssize = 0, n, i, j, tid;
  unsigned short int count = 0, rcount = 0;
  unsigned char filebuf[MAXDATASIZE + 1];
  unsigned char packetbuf[MAXDATASIZE + 12],
    recvbuf[MAXDATASIZE + 12];
  char *bufindex;	//fullpath[196],
  struct sockaddr_in ack;
  int sock;
  uint32_t flashaddr = fileinfo->flashaddr;
  struct sockaddr_in server;
  unsigned short int ackfreq = 1;
  int errno;
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = ipaddr;
  server.sin_port = htons((uint16_t)69);

  /*Create the socket, a -1 will show us an error */
    if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      {
        return -1;
      }
  
    memset (filebuf, 0, MAXDATASIZE);  /*clear the buffer */
    /* this is the first request message */ 
    len = req_packet (opcode, fileinfo->filename, "octet", filebuf);
    server_len = sizeof(struct sockaddr);
  
    if (sendto (sock, filebuf, len, 0, (struct sockaddr *) &server, server_len) !=
        len)
      {
        close(sock);
        return -2;
      }

/*At this point I have to wait to recieve an ACK from the server before I start sending the file*/
  /*open the file to read */
//get ACK for WRQ
/* The following 'for' loop is used to recieve/timeout ACKs */
  for (j = 0; j < RETRIES - 2; j++)
    {
      server_len = sizeof (ack);
      errno = EAGAIN;
      n = -1;
      for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)
	  {
	    n = recvfrom (sock, recvbuf, sizeof (recvbuf), MSG_DONTWAIT,
			(struct sockaddr *) &ack, (socklen_t *) & server_len);
	    mico_rtos_delay_milliseconds(1);
	  }




      tid =  (ack.sin_port);	//get the tid of the server.
      server.sin_port =  (tid);	//set the tid for rest of the transfer

      if (n < 0) {

      }
      else
	{			/*changed client to server here */
	  if (server.sin_addr.s_addr != ack.sin_addr.s_addr )	/* checks to ensure send to ip is same from ACK IP */
	    {
	      j--;		/* in this case someone else connected to our port. Ignore this fact and retry getting the ack */
	      continue;
	    }
	  if (tid != (server.sin_port))	/* checks to ensure get from the correct TID */
	    {
	      len = err_packet (5, err_msg[5], filebuf);
	      if (sendto (sock, filebuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
		{
		}
	      j--;
	      continue;		/* we aren't going to let another connection spoil our first connection */
	    }

/* this formatting code is just like the code in the main function */
	  bufindex = (char *) recvbuf;	//start our pointer going
	  if (bufindex++[0] != 0x00) {
      }
	  opcode = *bufindex++;

	  rcount = *bufindex++ << 8;
	  rcount &= 0xff00;
	  rcount += (*bufindex++ & 0x00ff);
	  if (opcode != 4 || rcount != count)	/* ack packet should have code 4 (ack) and should be acking the packet we just sent */
	    {

/* sending error message */
	      if (opcode > 5)
		{

		  len = err_packet (4, err_msg[4], filebuf);
		  if (sendto (sock, filebuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
		    {
		    }
		}
	      /* from here we will loop back and resend */
	    }
	  else
	    {
	      break;
	    }
	}			//end of else

    }
/* The ack sending 'for' loop ends here */



  memset (filebuf, 0, sizeof (filebuf));	//clear the filebuf
  while (1)			/* our break statement will escape us when we are done */
    {
      //acked = 0;
      if (fileinfo->filelen > MAXDATASIZE)
        ssize = MAXDATASIZE;
      else
        ssize = fileinfo->filelen;
      fileinfo->filelen -= ssize;
      MicoFlashRead(fileinfo->flashtype, &flashaddr, (uint8_t *)filebuf, ssize);


      count++;			/* count number of datasize byte portions we read from the file */

	  packetbuf[0] = 0x00;
	  packetbuf[1] = 0x03;
      packetbuf[2] = (count & 0xFF00) >> 8;	//fill in the count (top number first)
      packetbuf[3] = (count & 0x00FF);	//fill in the lower part of the count
      memcpy ((char *) packetbuf + 4, filebuf, ssize);
      len = 4 + ssize;
      /* send the data packet */
      if (sendto
	  (sock, packetbuf, len, 0, (struct sockaddr *) &server,
	   sizeof (server)) != len)
	{
      close(sock);
	  return -3;
	}
      //if ((count - 1) == 0 || ((count - 1) % ackfreq) == 0 || ssize != datasize)
      if (((count) % ackfreq) == 0 || ssize != MAXDATASIZE)
	{
/* The following 'for' loop is used to recieve/timeout ACKs */
	  for (j = 0; j < RETRIES; j++)
	    {
	      server_len = sizeof (ack);
	      errno = EAGAIN;
	      n = -1;
	      for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)
		{
		  n =
		    recvfrom (sock, recvbuf, sizeof (recvbuf), MSG_DONTWAIT,
			      (struct sockaddr *) &ack,
			      (socklen_t *) & server_len);
		  mico_rtos_delay_milliseconds(1);
		}

          if (n < 0) {

      }
	      else
		{		/* checks to ensure send to ip is same from ACK IP */
		  if (server.sin_addr.s_addr != ack.sin_addr.s_addr )
		    {
		      /* in this case someone else connected to our port. Ignore this fact and retry getting the ack */
		      j--;
		      continue;
		    }
		  if (tid !=  (server.sin_port))	/* checks to ensure get from the correct TID */
		    {
		      len = err_packet (5, err_msg[5], filebuf);
		      /* send the data packet */
		      if (sendto
			  (sock, filebuf, len, 0, (struct sockaddr *) &server,
			   sizeof (server)) != len)
			{
			}
		      j--;

		      continue;	/* we aren't going to let another connection spoil our first connection */
		    }

/* this formatting code is just like the code in the main function */
		  bufindex = (char *) recvbuf;	//start our pointer going
		  if (bufindex++[0] != 0x00) {
          }
		  opcode = *bufindex++;

		  rcount = *bufindex++ << 8;
		  rcount &= 0xff00;
		  rcount += (*bufindex++ & 0x00ff);
		  if (opcode != 4 || rcount != count)	/* ack packet should have code 4 (ack) and should be acking the packet we just sent */
		    {
		      /* sending error message */
		      if (opcode > 5)
			{
			  len = err_packet (4, err_msg[4], filebuf);
			  if (sendto (sock, filebuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
			    {
			    }
			}
		      /* from here we will loop back and resend */
		    }
		  else
		    {
		      break;
		    }
		}
	      
		{
		  if (sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* resend the data packet */
		    {
              close(sock);
		      return -4;
		    }
		}

	    }
/* The ack sending 'for' loop ends here */

	}


      if (j == RETRIES)
	{
        close(sock);
	  return -5;
	}
      if (ssize != MAXDATASIZE)
	break;

      memset (filebuf, 0, sizeof (filebuf));	/* fill the filebuf with zeros so that when the fread fills it, it is a null terminated string */
    }

  close(sock);
  return 0;
}				//end of tsend function


/*
*This function is called when the client would like to download a file from the server. Return the downloaded file length
*/
int
tget (tftp_file_info_t *fileinfo, uint32_t ipaddr)
{
  /* local variables */
  int len, server_len, opcode = RRQ, i, j, n, tid = 0, flag = 1;
  unsigned short int count = 0, rcount = 0;
  unsigned char filebuf[MAXDATASIZE + 12];
  unsigned char packetbuf[128];
  int errno;
  char *bufindex;
  struct sockaddr_in data;
  int sock;
  uint32_t flashaddr = fileinfo->flashaddr;
  struct sockaddr_in server;
  //unsigned short int ackfreq = 1;
  int totalen = 0;
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = ipaddr;
  server.sin_port = htons((uint16_t)69);
  
  /*Create the socket, a -1 will show us an error */
    if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      {
        return -1;
      }

   MicoFlashErase(fileinfo->flashtype,fileinfo->flashaddr,fileinfo->filelen);
  
    memset (packetbuf, 0, sizeof(packetbuf));  /*clear the buffer */
    /* this is the first request message */
    len = req_packet (opcode, fileinfo->filename, "octet", packetbuf);
    server_len = sizeof(struct sockaddr);
  
    sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, server_len);
  n = MAXDATASIZE + 4;
  do
    {
      /* zero buffers so if there are any errors only NULLs will be exposed */
      memset (filebuf, 0, sizeof (filebuf));
    

      for (j = 0; j < RETRIES; j++)	/* this allows us to loop until we either break out by getting the correct ack OR time out because we've looped more than RETRIES times */
	{
	  server_len = sizeof (data);
	  errno = EAGAIN;	/* this allows us to enter the loop */
	  n = -1;
	  for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)	/* this for loop will just keep checking the non-blocking socket until timeout */
	    {

	      n =
		recvfrom (sock, filebuf, sizeof (filebuf) - 1,
			  MSG_DONTWAIT, (struct sockaddr *) &data,
			  (socklen_t *) & server_len);
	      mico_rtos_delay_milliseconds(1);
	    }
	  if (!tid)
	    {
	      tid =  (data.sin_port);	//get the tid of the server.
	      server.sin_port =  (tid);	//set the tid for rest of the transfer
	    }

      if (n < 0) {
        sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, server_len);
      }
	  
	  else
	    {
	      if (server.sin_addr.s_addr != data.sin_addr.s_addr)	/* checks to ensure get from ip is same from ACK IP */
		{
		  j--;
		  continue;	/* we aren't going to let another connection spoil our first connection */
		}
	      if (tid !=  (server.sin_port))	/* checks to ensure get from the correct TID */
		{
		  packetbuf[0] = 0x00;
		  packetbuf[1] = 0x05;
		  packetbuf[2] = 0x00;
		  packetbuf[3] = 0x05;
		  len = strlen("Bad/Unknown TID");
		  memcpy(&packetbuf[4], "Bad/Unknown TID", len);
		  len += 4;
		  packetbuf[len++] = 0x00;
		  if (sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
		    {
		    }
		  j--;
		  continue;	/* we aren't going to let another connection spoil our first connection */
		}
/* this formatting code is just like the code in the main function */
	      bufindex = (char *) filebuf;	//start our pointer going
	      if (bufindex++[0] != 0x00) {
          }
	      opcode = *bufindex++;
	      rcount = *bufindex++ << 8;
	      rcount &= 0xff00;
	      rcount += (*bufindex++ & 0x00ff);
	      if (flag)
		{
		  flag = 0;
		}
	      if (opcode != 3)	/* ack packet should have code 3 (data) and should be ack+1 the packet we just sent */
		{
/* sending error message */
		  if (opcode > 5)
		    {
			  packetbuf[0] = 0x00;
	   		  packetbuf[1] = 0x05;
	   		  packetbuf[2] = 0x00;
	   		  packetbuf[3] = 0x04;
	   		  len = strlen("Illegal operation");
	   		  memcpy(&packetbuf[4], "Illegal operation", len);
	   		  len += 4;
	   		  packetbuf[len++] = 0x00;
		      if (sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
			{
			}
		    }
		}
	      else
		{
          if (count + 1 == rcount) {// received right seq no, save and ack
            MicoFlashWrite(fileinfo->flashtype, &flashaddr, (uint8_t *)bufindex, n - 4);
            totalen += n-4;
          } 

          len = 4;
		  packetbuf[0] = 0x00;
   		  packetbuf[1] = 0x04;
		  packetbuf[2] = (rcount & 0xFF00) >> 8;	//fill in the count (top number first)
		  packetbuf[3] = (rcount & 0x00FF);	//fill in the lower part of the count
		  count = rcount;


          sendto(sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server));
		  break;
		}		//end of else
	    }
	}
      if (j == RETRIES)
	{
      close(sock);
	  return -4;
	}
      if (n != (MAXDATASIZE + 4))	/* remember if our datasize is less than a full packet this was the last packet to be received */
   	  {
   	    goto done;		/* gotos are not optimal, but a good solution when exiting a multi-layer loop */
   	  }
    }
  while (1);

done:

  close(sock);
  return totalen;
}
