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
			if((*pq)[i]){
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
			}
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

PCB * dePQ(PCBQueue * pq, PCB * toDQ) {  // Needs to send the first one found if toDQ = NULL (process_switch) otherwise dequeue normally
	//if(priority<0 || priority>3 || !pq || !pq[priority]){
	//	return NULL;
	//}else{
	int priority = 0;
	if (!toDQ) { //If no specific PCB is sent in for dequeuing
		while (pq[priority] == NULL) { // Going through each priority level and checking
			priority++;
			if (priority > 3) //Should NEVER reach here...null_process should always be running!
				k_terminate ();
		}
		PCB * pcb = pq[priority]->head;
		if(!(pcb->nextPCB)){
			pq[priority]->tail = NULL;
		}
		pq[priority]->head = pcb->nextPCB;
		(pq[priority]->size)--;
		return pcb;
	// MAJOR DEBUGGING NEEDED HERE PRETTY SURE THAT I DID NOT ACCOUNT FOR MANY CASES!
	} else { // Second case we are looking for a specific PCB to dequeue
		priority = 0;
		PCB *temp = pq[priority]->head;
		int condition = 0;
		if (temp->id == toDQ->id) //The first one was it!
			condition = 1;
		while ((priority < 4) && (!condition)){
			while (temp) {
				temp = temp->nextPCB;
				if (temp->nextPCB->id == toDQ->id) { // Found the previous PCB
					condition = 1;
				}
			}
			priority ++;
			temp = pq[priority]->head;
			if (temp->id == toDQ->id) // The first one was it!
						condition = 1;
		}
		temp->nextPCB = toDQ->nextPCB;
	}
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

int queueSize (PCBQueue *pq) {
	if (!pq) {	// if the queue sent in is NULL
		return 0;
	} else { // Go through all the levels
		int processCount = 0, i = 0;
		while (pq[i]) {
			processCount += pq[i]->size; // Adding the number of processes at that level
		}
		return (processCount); // Returning the total number of processes in that list
	}
}
