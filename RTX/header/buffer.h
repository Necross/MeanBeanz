/*
 * buffer.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "global.h"
#include <setjmp.h>

//UART buffer; a string; can be used as input/output buffer
typedef struct char_buf{
	char value[MAX_BUFFER_SIZE];
	//position of next char(to write)
	int pos;
	int size;
	//flag if it is full
	int ok_flag;
} UARTBuf;

//put chars in buffer
int enBuf(UARTBuf * buf, char * contentPtr);

//Make a copy of the buffer with only value and actual length of char
int bufCopy(UARTBuf * buf, char * copy);

//Jump Buffer; it is a pre-defined C-Standard-Library type
typedef jmp_buf JmpBuf;

//Trace Buffer
typedef struct trace_buf{
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
} TraceBuffer;

//Initializing the buffer
int traceBufferInit(TraceBuffer * traceBuf);

//Destroy the buffer
int traceBufferDestroy(TraceBuffer * traceBuf);

//save a new message trace
TraceBuffer * saveTrace(MsgEnv * msgEnv);

//add a newTrace to traceBuf list
TraceBuffer * addTrace(TraceBuffer * traceBuf, TraceBuffer * newTrace);


#endif /* BUFFER_H_ */
