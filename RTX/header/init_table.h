/*
 * init_table.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef INIT_TABLE_H_
#define INIT_TABLE_H_

#include "global.h"

typedef struct pcb_record{
	int id;
	int priority;
	int stackSize;
	PC pc;
} InitTable;

int initTableInit(InitTable * iT, int id, int priority, int size, PC pc);

int initTableDestroy(InitTable * iT);

#endif /* INIT_TABLE_H_ */
