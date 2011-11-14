/*
 * pcb.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef PCB_H_
#define PCB_H_

#include <setjmp.h>
#include "global.h"
#include "msg.h"


struct pcb{
	//This is also a linked list
	struct pcb *nextPCB;
	//State of the process
	PCBState state;
	//Process ID
	int id;
	int priority;
	int pc;
	int * sp;
	//Context of the process(for process switching)
	jmp_buf jbContext;
	//Received Message Envelope Queue(Priority Queue)
	struct msg_env * msgEnvQueue;
	//Atomic Count
	long aCount;
	//Additional Information Field
	void* addField;
};

//Initialization and memory allocation
int pcbInit(PCB ** pcbInstance, int id, int priority, int pc, int stkSize, PCBState state);

//Destroy and free the memory for the process
int pcbDestroy(PCB ** pcbInstance);

#endif /* PCB_H_ */
