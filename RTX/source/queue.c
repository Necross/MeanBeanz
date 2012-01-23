/*
 * queue.c
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#include "queue.h"


int PCBQueueInit(PCBQueue ** pq){
	*pq = (PCBQueue *)malloc(4 * sizeof(PCBQueue));
	int i;
	for(i=0; i<4 ; i++){
		(*pq)[i]=(PCBList *)malloc(sizeof(PCBList));
		(*pq)[i]->size = 0;
	}
	if(!(*pq)){
		return 0;
	}else{
		int i;
		for(i=0; i<4; i++){
			if(!(*pq)[i]){
				return 0;
			}
		}
		return 1;
	}
}

int PCBQueueDestroy(PCBQueue ** pq){
	if(*pq){
		int i;
		for(i=0; i<4; i++){
			/*if((*pq)[i]){
				PCB * current = (*pq)[i]->head;
				PCB * next = NULL;
				while(current){
					next = current->nextPCB;
					free(current);
					current = next;
				}
				(*pq)[i]->head = NULL;
				(*pq)[i]->tail = NULL;
				(*pq)[i]->size = 0;
			}*/
			free((*pq)[i]);
		}
		free(*pq);
		*pq = NULL;
	}
	if(*pq){
		return 0;
	}else{
		return 1;
	}
}

int enPQ(PCBQueue * pq, PCB * pcb, int priority){
	if(priority<0 || priority>3 || pcb==NULL || pq==NULL || pq[priority]==NULL){
		return 0;
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
		return 1;
	}
}

PCB * dePQ(PCBQueue * pq, PCB * proc){
	if(!pq){ // If out of bounds
		return NULL;
	} else {
		if(proc == NULL){
			int i;
			PCB * p;
			for(i=0; i<4; i++){
				p = pq[i]->head;
				if(p){
					pq[i]->head = pq[i]->head->nextPCB;
					if(p == pq[i]->tail)
						pq[i]->tail = NULL;
					pq[i]->size--;
					p->nextPCB = NULL;
					return p;
				}
			}
		}else{
			PCB * pCurrent, * pcb;
			int i;
			for(i=0; i<4; i++){
				pCurrent = pq[i]->head;
				if(pq[i]->head == proc && pq[i]->tail == proc){
					pq[i]->head = NULL;
					pq[i]->tail = NULL;
					pcb = pCurrent;
				}else if(pq[i]->head == proc && pq[i]->tail != proc){
					pcb = pq[i]->head;
					pq[i]->head = pq[i]->head->nextPCB;
				}else{
					while(pCurrent != NULL && pCurrent->nextPCB != proc)
						pCurrent = pCurrent->nextPCB;
					if(pCurrent != NULL){
						pcb = pCurrent->nextPCB;
						if(pcb == pq[i]->tail)
							pq[i]->tail = pCurrent;
						pCurrent->nextPCB = pCurrent->nextPCB->nextPCB;
					}

				}
				if(pcb){
					pq[i]->size--;
					return pcb;
				}
			}
			return NULL;
		}

	}
	return NULL;
}

int resortPQ(PCBQueue * pq, PCB * pcb, int priority){
	if(priority<0 || priority>3 || !pq || !pq[priority]){
		return 0;
	}else{
		PCB * prev = pq[priority]->head;
		while(prev && prev->nextPCB != pcb)
			prev = prev->nextPCB;
		if(prev)
			prev->nextPCB = prev->nextPCB->nextPCB;
		return enPQ(pq, pcb, priority);
	}
}

PCB * SearchQueue(PCBQueue * pq, int pid){
	if(pid<0 || !pq)
		return 0;
	int i;
	for(i=0; i<4; i++){
		if(pq[i]){
			PCB * current = pq[i]->head;
			while(current && current->nextPCB && current->id != pid)
				current = current->nextPCB;
			if(current && current->id == pid)
				return current;
		}
	}
	return NULL;
}
