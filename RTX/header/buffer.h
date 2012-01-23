#ifndef BUFFER_H_
#define BUFFER_H_

#include <setjmp.h>
#include "global.h"
#include "msg.h"

//UART buffer; a string; can be used as input/output buffer
struct char_buf{
       //position of next char(to write)
    int pos;
    int size;
    //flag if it is full
    int ok_flag;
    char value[MAX_BUFFER_SIZE];
};

//Trace Buffer
struct tbn{
    //Sender and Receiver PID
    int destID;
    int senderID;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CONSTANT?
    int size;
    MsgType msgType;
    //Time at tracing
    long timeStamp;
};

struct trace_buf {
   TraceBufferNode tbn[MAX_TRACE_SIZE];
   int tail;
};


    //Destroy the buffer
int traceBufferDestroy(TraceBuffer * traceBuf);

//save a new message trace
//TraceBufferNode * saveTrace(MsgEnv * msgEnv);

//add a newTrace to traceBuf list
//TraceBufferNode * addTrace(TraceBuffer * traceBuf, TraceBuffer * newTrace);

#endif /* BUFFER_H_ */
