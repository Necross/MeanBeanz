/*
 * queue.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "global.h"
#include "pcb.h"

//Array Element
typedef struct pcb_list{
	//Pointer to the head of the PCB list
	PCB * head;
	//Pointer to the tail of the PCB list
	PCB * tail;
	//Size of the single PCB list
	int size;
} PCBList;

//Queue is a dynamic array of PCB list
typedef PCBList * PCBQueue;

//Initializing the queue
int PCBQueueInit(PCBQueue ** pq);

//Destroy the queue
int PCBQueueDestroy(PCBQueue ** pq);

//Enqueue a PCB in the queue with priority;
//for non-priority PCB, it is default 3 (lowest priority)
int enPQ(PCBQueue * pq, PCB * pcb, int priority);

//Dequeue a PCB from queue;
//if PID is not specified, it is default -1, and
//the most front process with highest priority will be dequeued
PCB * dePQ(PCBQueue * pq, PCB * proc);

//Put PCB in a new place where as the priority
int resortPQ(PCBQueue * pq, PCB * pcb, int priority);

//Get PCB pointer from a queue searching by process id
PCB * SearchQueue(PCBQueue * pq, int pid);

#endif /* QUEUE_H_ */
