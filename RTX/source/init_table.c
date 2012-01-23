/*
 * init_table.c
 *
 *  Created on: Nov 9, 2011
 *      Author: zik
 */

#include "init_table.h"

int initTableInit(InitTable * iT, int id, int priority, ProcessType pType, int stkSize, void * pc){
	//Assuming it is malloc-ed because we are using the struct
	iT->id = id;
	iT->priority = priority;
	iT->pType = pType;
	iT->stackSize = stkSize;
	iT->pc = pc;
	if(iT){
		return 1;
	}else{
		return 0;
	}
}

int initTableDestroy(InitTable * iT){
	free(iT);
	if(iT){
		return 0;
	}else{
		return 1;
	}
}
