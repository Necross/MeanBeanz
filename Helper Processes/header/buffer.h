/*
 * buffer.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <setjmp.h>
#include "global.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>
//#include "msg.h"

//UART buffer; a string; can be used as input/output buffer
struct char_buf{
	char value[MAX_BUFFER_SIZE];
	//position of next char(to write)
	int pos;
	int size;
	//flag if it is full
	int ok_flag;
};

//put chars in buffer
int enBuf(UARTBuffer * buf, char * contentPtr);

//Make a copy of the buffer with only value and actual length of char
int bufCopy(UARTBuffer * buf, char * copy);

//Jump Buffer; it is a pre-defined C-Standard-Library type
typedef jmp_buf JmpBuf;

//Trace Buffer
struct trace_buf{
	//Trace buffer is a two way linked list
	struct trace_buf *prev;
	struct trace_buf *next;
	//Sender and Receiver PID
	int destID;
	int senderID;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CONSTANT?
	int size;
	MsgType msgType;
	//Time at tracing
	long timeStamp;
};

//Initializing the buffer
int traceBufferInit(TraceBuffer * traceBuf);

//Destroy the buffer
int traceBufferDestroy(TraceBuffer * traceBuf);

//save a new message trace
TraceBuffer * saveTrace(MsgEnv * msgEnv);

//add a newTrace to traceBuf list
TraceBuffer * addTrace(TraceBuffer * traceBuf, TraceBuffer * newTrace);


#endif /* BUFFER_H_ */
