#include "global.h"
#include "msg.h"
#include "buffer.h"
#include "k_rtx.h"

extern k_RTX * kernel;
int enTraceBuf(TraceBuffer * traceBuf, MsgEnv * msgEnv)
{
   if ((traceBuf == NULL) || (msgEnv == NULL))
       return 1;
   int i= traceBuf->tail;
   (*traceBuf).tbn[i].senderID = msgEnv->senderID;
   (*traceBuf).tbn[i].destID = msgEnv->destID;
   (*traceBuf).tbn[i].msgType = msgEnv->type;
   (*traceBuf).tbn[i].timeStamp = kernel->totalTicks;
   if (i==(MAX_TRACE_SIZE -1))
       traceBuf->tail=0;
   else
       traceBuf->tail ++;
   return 0;
}

int getTraceBuf(MsgEnv * msgEnv)
{

	int i = 0,  k;
	char j= 0;
   j= kernel->firstSent->tail-1;

   if (j==-1)
	   j=15;

   for (k=0 ; k<MAX_TRACE_SIZE;k++)
   {
       msgEnv->msg[i++] = (*(kernel->firstSent)).tbn[j].destID;
       msgEnv->msg[i++] = (*(kernel->firstSent)).tbn[j].senderID;
       msgEnv->msg[i++] = (*(kernel->firstSent)).tbn[j].msgType;
      	msgEnv->msg[i++]= j;
       if (j==0)
               j=MAX_TRACE_SIZE-1;
       else
               j--;
   }
   j=kernel->firstReceived->tail-1;
   for (k=0 ; k<MAX_TRACE_SIZE;k++)
   {
        msgEnv->msg[i++] = (*(kernel->firstReceived)).tbn[j].destID;
        msgEnv->msg[i++] = (*(kernel->firstReceived)).tbn[j].senderID;
        msgEnv->msg[i++] = (*(kernel->firstReceived)).tbn[j].msgType;
     
        msgEnv->msg[i++]= j;

        if (j==0)
                j=MAX_TRACE_SIZE-1;
        else
                j--;
        }
   return 0;
}


int traceBufferDestroy(TraceBuffer * traceBuf)
{
   free(traceBuf);
   if (traceBuf){return 1;}
   return 0;
}
