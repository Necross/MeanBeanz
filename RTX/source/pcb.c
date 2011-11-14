/*
 * pcb.c
 *
 *  Created on: Nov 9, 2011
 *      Author: zik
 */

#include "pcb.h"

int pcbInit(PCB ** pcbInstance, int id, int priority, int stkSize, int pc, PCBState state){
	if(*pcbInstance){
		return 0;
	}else{
		*pcbInstance = (PCB *)malloc(sizeof(PCB));
		(*pcbInstance)->id = id;
		(*pcbInstance)->priority = priority;
		(*pcbInstance)->pc = pc;
		(*pcbInstance)->state = state;
		(*pcbInstance)->sp = (int *)malloc(stkSize * sizeof(int));
		(*pcbInstance)->aCount = 0;
		return 1;
	}
}

int pcbDestroy(PCB ** pcbInstance){
	free((*pcbInstance)->addField);
	if((*pcbInstance)->addField){
		return 0;
	}else{
		free(*pcbInstance);
		if(*pcbInstance){
			return 0;
		}else
			return 1;
	}
}
