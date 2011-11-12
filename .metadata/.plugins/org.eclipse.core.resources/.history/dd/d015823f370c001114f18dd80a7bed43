/*
 * init_table.c
 *
 *  Created on: Nov 9, 2011
 *      Author: zik
 */

#include "init_table.h"

int initTableInit(InitTable * iT, int id, int priority, int stkSize, int pc){
	iT = (InitTable *)malloc(sizeof(InitTable));
	iT->id = id;
	iT->priority = priority;
	iT->stackSize = stkSize;
	iT->pc = pc;
	if(iT){
		return 0;
	}else{
		return -1;
	}
}

int initTableDestroy(InitTable * iT){
	free(iT);
	if(iT){
		return -1;
	}else{
		return 0;
	}
}
