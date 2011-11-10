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
#include "msg_env.h"

//The list of states of process:
//READY: ready to execute; EXECUTING: running; SUSPENDED: execution suspended;
//INTERRUPTED: interrupted by signal/envelope; BLOCK_ON_RCV: blocked on receiving signal/message
//BLOCK_ON_ENV: blocked on no available envelope;
//IS_IPROCESS: it is an iProcess
typedef enum pcb_state{
	READY, EXECUTING, SUSPENDED, INTERRUPTED,
	BLOCK_ON_RCV, BLOCK_ON_ENV,
	IS_IPROCESS, NO_BLK_RCV
} PCBState;

typedef struct pcb{
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
	MsgEnv * msgEnvQueue;
	//Atomic Count
	long aCount;
	//Additional Information Field
	void* addField;
} PCB;

//Initialization and memory allocation
int pcbInit(PCB * pcbInstance, int id, int priority, int pc, int stkSize, PCBState state);

//Destroy and free the memory for the process
int pcbDestroy(PCB * pcbInstance);

#endif /* PCB_H_ */
