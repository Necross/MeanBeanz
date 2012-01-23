/*
 * pcb.c
 *
 *  Created on: Nov 9, 2011
 *      Author: zik
 */

#include "pcb.h"

int pcbInit(PCB ** pcbInstance, int id, int priority, void * pc, PCBState state){
	*pcbInstance = (PCB *)malloc(sizeof(PCB));
	(*pcbInstance)->id = id;
	(*pcbInstance)->priority = priority;
	(*pcbInstance)->pc = pc;
	(*pcbInstance)->state = state;
	(*pcbInstance)->sp = (char *)malloc(MAX_STACK_SIZE);
	(*pcbInstance)->StkSize = MAX_STACK_SIZE;
	return 1;
}

int pcbDestroy(PCB ** pcbInstance){
	free((*pcbInstance)->sp);
	if((*pcbInstance)->sp){
		return 0;
	}else{
		free(*pcbInstance);
		if(*pcbInstance){
			return 0;
		}else
			return 1;
	}
}
