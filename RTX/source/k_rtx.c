/*
 * rtx.c
 *
 *  Created on: 2011-11-07
 *      Author: Sohaib
 */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "k_rtx.h"

extern k_RTX * kernel;

/********************************HELPER FUNCTIONS***************************************************/

PCB * getPCB(int pid){ //get pcb from pid
	PCB * p = SearchQueue(kernel->rq, pid);
	if(!p)
		p = SearchQueue(kernel->bq, pid);
	return p;
}



/********************************iProcesses (KB, CRT AND TIMER)**********************************/
//The timer i_process
void k_timer_iProcess () {
	k_atomic (on); //turning off interrupts
	PCB * old_Process = kernel->current_process;
	kernel->current_process = getPCB (kernel->timer_pid);	//Changing current process because thats how send_msg works
	MsgEnv * temp, * newMsg, * previous;
	PCB *timerPCB = getPCB (kernel->timer_pid);
	newMsg = k_receive_message (); // WARNING: What if more than one message is received???
	//Perhaps make sure that the newMsg type is the right one? REQUEST_DELAY type! Return envelope unless envelope is right one
	if (!newMsg) {	//If there was a new message
		temp = timerPCB->msgEnvQueue;
		previous = temp;
		while (!(temp->nextMsgEnv)) { // This should never be since the wallclock counter is always here!
		if (newMsg->msg.timeoutCount <= temp->msg.timeoutCount) {	//Adding the new message
			 previous->nextMsgEnv = newMsg;
			 newMsg->nextMsgEnv = temp;
		 }
	}
	//Decrementing the timer for all messages
	temp = timerPCB->msgEnvQueue;
	if (temp) // Only go in if there are envelopes to dequeue from
		while (!(temp->nextMsgEnv)) {
			temp->msg.timeoutCount--;
			if (temp->msg.timeoutCount <= 0) {
				temp->type = WAKEUP_CODE; //Setting type to wakeup code before returning it
				k_send_message(temp->senderID, temp); //Remove from queue? Take care in send_message?
			}
			temp = temp->nextMsgEnv;
		}
	}
	kernel->current_process = old_Process; //Restoring old process
	k_atomic (off);
}

//crt-iprocess check local buffer if its ready, copy data to share memory and reset local buffer
void k_crt_iProcess() {
	if(kernel->crt_buf->ok_flag == 1){
		int i = 0;
		UARTBuffer * share_mem = (UARTBuffer *)(kernel->crt_mem);
		while((share_mem->value[i] = kernel->crt_buf->value[i]))
			i++;
		share_mem->ok_flag = 1;
		share_mem->size = i;
		kernel->crt_buf->size = 0;
		kernel->crt_buf->ok_flag = 0;
	}
}

void k_kb_iProcess() {
	UARTBuffer * share_mem = (UARTBuffer *)(kernel->kb_mem);
	if(share_mem->ok_flag ==1){
		int i = 0;
		while((kernel->kb_buf->value[i] = share_mem->value[i]))
			i++;
		kernel->kb_buf->ok_flag = 1;
		kernel->kb_buf->size = i;
		share_mem->size = 0;
		share_mem->ok_flag = 0;
	}
}



/****************************CRT AND KB PROCESS COMMUNICATION*********************/

int k_send_console_chars(MsgEnv * message_envelope ){
	message_envelope->senderID = kernel->current_process->id;
	if(message_envelope->msg.str){

		//Copy the chars to be sent into local crt buffer
		//and invoke crt_iprocess to copy the data from local buffer to share memory
		int i = 0;
		while((kernel->crt_buf->value[i] = message_envelope->msg.str->value[i]))
			i++;
		kernel->crt_buf->ok_flag = 1;
		kernel->crt_buf->size = i;
		if(kill(getpid(),SIGUSR2) != 0)
			return 0;
		return k_send_message(message_envelope->senderID, message_envelope);
	}else{
		k_send_message(message_envelope->senderID, message_envelope);
		return 0;
	}
}

int k_get_console_chars(MsgEnv * message_envelope ){
	while(kernel->kb_buf->ok_flag == 0)
		usleep(100000);

	//Wait till the buf is ok, and copy the data from local buffer to envelope
	//and reset local buffer
	message_envelope->senderID = kernel->current_process->id;
	int i = 0;
	message_envelope->msg.str = (UARTBuffer *)malloc(sizeof(UARTBuffer));
	while((message_envelope->msg.str->value[i] = kernel->kb_buf->value[i]))
		i++;
	kernel->kb_buf->ok_flag = 0;
	kernel->kb_buf->size = 0;
	return k_send_message(message_envelope->senderID, message_envelope);
}



/************************************ATOMICITY AND KERNEL DESTRUCTION***************************/

void k_atomic(a_switch flip) {
/*
	//extern static long aCount;
	static sigset_t oldmask; //Static set of signals
	sigset_t newmask; //Newmask will be used to mask and unmask
	if (flip == on) {
		if (kernel->current_process->aCount == 0) { //Error check, multiple executions of atomic prohibited
			(kernel->current_process->aCount)++; //Turning the atomic indicator in the PCB on
			sigemptyset(&newmask); //Creating a new signal mask
			sigaddset(&newmask, SIGUSR1);
			sigaddset(&newmask, SIGUSR2);
			sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		}else{
			kernel->current_process->aCount++;
		}
	} else { //The signals are turned back on!
		if (kernel->current_process->aCount == 1) {
			(kernel->current_process->aCount)--; //Turning the atomic indicator in the PCB off
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}else if(kernel->current_process->aCount >1){
			(kernel->current_process->aCount)--;
		}
	}*/
}

int k_terminate () {
	die (); //Add destructors...copy over the stuff in main.c to over here...
}



/*********************************INTERPROCESS COMMUNICATION****************************************/

int k_request_delay(int time_delay, int wakeupCode, MsgEnv * msg_env) {
	if (!msg_env){
		msg_env->destID = kernel->timer_pid;
		msg_env->msg.timeoutCount = time_delay; //Time delay is multiples of 100 microSeconds
		msg_env->type = REQUEST_DELAY;
		k_send_message (kernel->timer_pid, msg_env);
		return 1;
	} else {
		return 0;
	}
}


MsgEnv * k_receive_message() {
	PCB * pcb = kernel->current_process;
	MsgEnv * result = pcb->msgEnvQueue; // Receiving the message envelope
	if ((!result) && (pcb->state == IS_IPROCESS)) {	// If the invoking process was an iProcess it does not get blocked
		return (NULL);
	} else if (!result) { // Otherwise if no message then the process gets blocked and a context switch occurs
		pcb->state = BLOCK_ON_RCV;
		enPQ (kernel->bq, pcb, 0); // Putting the process on the blocked queue, do we need this? Ask TA....
		k_process_switch (NULL); // Doing a process switch
	}
	// Will eventually return here. When process resumes execution it will execute the receive message line again!
	pcb->msgEnvQueue = result->nextMsgEnv;
	result->nextMsgEnv = NULL;
	return result;
}


int k_send_message(int dest_process_id, MsgEnv * msg_envelope){
	PCB * pcb = getPCB(dest_process_id);
	if(!pcb){
		return 0;
	}
	msg_envelope->senderID = kernel->current_process->id; //if request delay comes here then the current process is set!
	msg_envelope->destID = dest_process_id;
	MsgEnv * tail = pcb->msgEnvQueue;
	if (tail) {
		while(tail->nextMsgEnv) // Going to the end of the list of messages
			tail = tail->nextMsgEnv;
		tail->nextMsgEnv = msg_envelope;
	} else {
		pcb->msgEnvQueue = msg_envelope; // If there are no messages there
	}
	if ((pcb->state) = BLOCK_ON_RCV) { // If the destination process was blocked on receive
		dePQ (kernel->bq, pcb); //Dequeuing from the blocked queue
		enPQ (kernel->rq, pcb, pcb->priority); //Adding to ready queue
		pcb->state = READY;
	}
	return 1;
}

MsgEnv * k_request_msg_env (){
	MsgEnv * env = kernel->availMsgEnvQueue->nextMsgEnv;
	if(!env){
		kernel->current_process->state = NO_BLK_RCV;
	}else{
		kernel->availMsgEnvQueue->nextMsgEnv = kernel->availMsgEnvQueue->nextMsgEnv->nextMsgEnv;
		env->nextMsgEnv = NULL;
		eraseMsg(env);
	}
	return env;
}




/***********************SCHEDULING AND PROCSSS SWITCHING**************************************** */

// WARNING: context of both processes must already have been set in initialisation
void k_context_switch (jmp_buf *oldProc, jmp_buf *newProc) { // Has to be a void what if null_proc -> null_proc
	int return_code  = setjmp(*oldProc); // return_code is 1 if the context of the old process was saved
	if (return_code == 0) {
		longjmp(*newProc,1); // Restoring the context of the new process 1 is sent to make the if condition fail the second time
	}
}


int k_process_switch (PCB *newProc) {
	if (!newProc) { // NULL Pointer was sent in
		return (0);
	} else  { // Valid pointer was sent in
		PCB *oldProc = kernel->current_process;
		if(!newProc) { 	//if newProc not specified we dequeue from the ready queue
			newProc = dePQ (kernel->rq, NULL);
			// Maybe take care of a case here?? Terminate kernel if unable to dequeue a process???? NULL return?
		}
		newProc->state = EXECUTING;
		kernel->current_process = newProc;
		k_context_switch (oldProc->jbContext, newProc->jbContext); // Sending context to context_switch
		return (1);
	}
}

int k_release_processor ()
{
	kernel->current_process->state = READY;
	enPQ(kernel->rq, kernel->current_process, kernel->current_process->priority); //Putting current process on Ready queue
	k_process_switch (NULL); //Sending in a NULL because we don't want to switch to a particular process
	//Return something????
}

void k_null_process () {
	while (true) {
		k_release_processor ();
	}
}



/***************************OTHER PRIMITIVES**********************************************************/

// Take care of a few things... current_process allowed here? Only Ready processes? Ask TA
int k_change_priority(int new_priority, int target_process_id) {
	int toReturn;
	if (new_priority>3  || new_priority<0) { // If the new priority was out of bounds
		printf ("KERNEL ERROR: Priority is out of bounds");
		toReturn = 0;
	} else {
		PCB *toChange = getPCB (target_process_id);
		if (!toChange) { // The specified PCB was NOT located!
			printf ("KERNEL ERROR: Process with specified pid does not exist.");
			toReturn = 0;
		} else if (new_priority != toChange->priority) { // The the priority really is to be changed
			dePQ (kernel->rq, toChange);
			toChange->priority = new_priority;
			toReturn = enPQ (kernel->rq, toChange, new_priority); // Should be 1 if successful
		} else {
			toReturn = 1; // The new priority is the same as the old one...no action required
		}
	}
	return toReturn;
}

// Returns a Tuple containing all the processes, first element of the tuple contains the number of elements...
int k_request_process_status (MsgEnv * msg_env_ptr) {
	if (!msg_env_ptr) { // If a NULL pointer was sent in
		return 0;
	} else { // Otherwise proceed as normal
		// Get number of processes
		int tupleSize = queueSize(kernel->bq) + queueSize(kernel->rq); // Getting total number of processes
		int i, priorityLevel = 0;
		PCB *temp;
		//Maybe add some error checks over here??
		TUPLE * procList[tupleSize+1]; // Adding one because top one only has the total number of tuples!
		for (i = 0; i < tupleSize+1; i++) {
			procList[i] = malloc (sizeof(TUPLE)); // Dynamically allocating each tuple in the array

		}
		i = 1;
		procList[0]->pid = tupleSize; // Sending the total number of tuples back in the pid of the first one
		while (kernel->rq[priorityLevel]) {	// Going through all the priority levels in the ready queue
			temp = kernel->rq[priorityLevel]->head;
			while (temp) {
				procList[i]->pid = temp->id;
				procList[i]->priority = temp->priority;
				procList [i]->procStat = temp->state;
				temp = temp->nextPCB;
			}
			priorityLevel++; // Moving onto the next priority level
		}
		priorityLevel = 0;
		while (kernel->bq[priorityLevel]) { // Going through all the priority levels in the blocked queue
			temp = kernel->bq[priorityLevel]->head;
			while (temp) {
				procList[i]->pid = temp->id;
				procList[i]->priority = temp->priority;
				procList [i]->procStat = temp->state;
				temp = temp->nextPCB;
			}
			priorityLevel++; // Moving onto the next priority level
		}
		msg_env_ptr->msg.processList = procList;
		int toReturn = k_send_message (msg_env_ptr->senderID, msg_env_ptr);
		return (toReturn);
	}
}


