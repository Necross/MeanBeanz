/*
 * init_table.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef INIT_TABLE_H_
#define INIT_TABLE_H_

#include "global.h"

struct pcb_record{
	int id;
	int priority;
	ProcessType pType;
	int stackSize;
	void * pc;
};

int initTableInit(InitTable * iT, int id, int priority, ProcessType pType, int stkSize, void * pc);

int initTableDestroy(InitTable * iT);

#endif /* INIT_TABLE_H_ */
