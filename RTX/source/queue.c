/*
 * queue.c
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#include "queue.h"


int PCBQueueInit(PCBQueue * pq){
	pq = (PCBQueue *)malloc(4 * sizeof(PCBQueue));
	for(int i=0; i<4 ; i++){
		pq[i]=(PCBList *)malloc(sizeof(PCBList));
		pq[i]->size = 0;
	}
	if(!pq){
		return -1;
	}else{
		for(int i=0; i<4; i++){
			if(!pq[i]){
				return -1;
			}
		}
		return 0;
	}
}

int PCBQueueDestroy(PCBQueue * pq){
	if(pq){
		for(int i=0; i<4; i++){
			if(pq[i]){
				PCB * current = pq[i]->head;
				PCB * next;
				while(current){
					next = current->nextPCB;
					free(current);
					if(current)
						return -1;
					current = next;
				}
				pq[i]->head = NULL;
				pq[i]->tail = NULL;
				pq[i]->size = 0;
			}
			free(pq[i]);
			if(pq[i])
				return -1;
		}
		free(pq);
	}
	if(pq){
		return -1;
	}else{
		return 0;
	}
}

int enPQ(PCBQueue * pq, PCB * pcb, int priority){
	if(priority<0 || priority>3 || !pcb || !pq || !pq[priority]){
		return -1;
	}else{
		if(pq[priority]->head == NULL){
			pq[priority]->head = pcb;
		}else{
			pq[priority]->tail->nextPCB = pcb;
		}
		pq[priority]->tail = pcb;
		(pq[priority]->size)++;
		pcb->nextPCB = NULL;
		pcb->priority = priority;
		return 0;
	}
}

PCB * dePQ(PCBQueue * pq, int priority){
	if(priority<0 || priority>3 || !pq || !pq[priority]){
		return NULL;
	}else{
		PCB * pcb = pq[priority]->head;
		if(!(pcb->nextPCB)){
			pq[priority]->tail = NULL;
		}
		pq[priority]->head = pcb->nextPCB;
		(pq[priority]->size)--;
		return pcb;
	}
}

int resortPQ(PCBQueue * pq, PCB * pcb, int priority){
	if(priority<0 || priority>3 || !pq || !pq[priority]){
		return -1;
	}else{
		PCB * prev = pq[priority]->head;
		while(prev && prev->nextPCB != pcb)
			prev = prev->nextPCB;
		if(prev)
			prev->nextPCB = prev->nextPCB->nextPCB;
		return enPQ(pq, pcb, priority);
	}
}

PCB * getPCB(PCBQueue * pq, int pid){
	if(pid<0 || !pq)
	for(int i=0; i<4; i++){
		if(pq[i]){
			PCB * current = pq[i]->head;
			while(current && current->nextPCB && current->nextPCB->id != pid)
				current = current->nextPCB;
			if(current)
				return current;
		}
	}
	return NULL;
}
