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
	MsgEnv *temp, *newMsg, *previous;
	PCB *timerPCB = getPCB (kernel->timer_pid);
	newMsg = k_receive_message (); // WARNING: What if more than one message is received???
	//Perhaps make sure that the newMsg type is the right one? REQUEST_DELAY type! is statement here?
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

MsgEnv * k_receive_message(){	//Return proper values for iProcesses
	//get the process own pcb
	//check state change
	PCB * pcb = kernel->current_process;
	if(pcb->state == IS_IPROCESS)
		return NULL;
	if(pcb->msgEnvQueue == NULL){
		pcb->state = NO_BLK_RCV;
		return NULL;
	}else{
		MsgEnv * result = pcb->msgEnvQueue;
		pcb->msgEnvQueue = result->nextMsgEnv;
		result->nextMsgEnv = NULL;
		return result;
	}
}

int k_send_message(int dest_process_id, MsgEnv * msg_envelope){
	//Dequeue from current owner maybe?
	//Change process state of receiving message

	PCB * pcb = getPCB(dest_process_id);
	if(!pcb){
		return 0;
	}
	msg_envelope->senderID = kernel->current_process->id; //if request delay comes here then the current process is set!
	msg_envelope->destID = dest_process_id;
	MsgEnv * tail = pcb->msgEnvQueue; // WARNING: Why is this called Tail?
	if (tail) {
		while(tail->nextMsgEnv)
			tail = tail->nextMsgEnv;
		tail->nextMsgEnv = msg_envelope;
	} else {
		pcb->msgEnvQueue = msg_envelope;
	}
	if (pcb->state = BLOCK_ON_RCV) { // If the destination process was blocked on receive
		dePQ (kernel->bq, pcb); //Dequeuing from the blocked queue
		enPQ (kernel->rq, pcb, pcb->priority); //Adding to ready queue
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
int k_context_switch (PCB *oldProc, PCB *newProc) {
	//use setjmp and longjmp to switch context etc....
	//return appropriate return codes over here....
}


int k_process_switch (PCB *newProc) {
	PCB *oldProc = kernel->current_process;
	if(!newProc) { 	//if newProc not specified we dequeue from the ready queue
		newProc = dePQ (kernel->rq, NULL);
	}
	k_context_switch (kernel->current_process, newProc);
	//Return codes go over here....
}


void k_null_process () {
	while (true) {
		k_release_processor ();
	}
}

int k_release_processor ()
{
	kernel->current_process->state = READY;
	enPQ(kernel->rq, kernel->current_process, kernel->current_process->priority); //Putting current process on Ready queue
	k_process_switch (NULL); //Sending in a NULL because we don't want to switch to a particular process
	//Return something????
}




/***************************OTHER PRIMITIVES**********************************************************/

// Take care of a few things... current_process allowed here? Only Ready processes? Ask TA
int k_change_priority(int new_priority, int target_process_id) {
	int toReturn;
	if (new_priority>3  || new_priority<0) { // If the new priority was out of bounds
		toReturn = 0;
	} else {
		PCB *toChange = getPCB (target_process_id);
		if (new_priority != toChange->priority) { // The the priority really is to be changed
			dePQ (kernel->rq, toChange);
			toChange->priority = new_priority;
			toReturn = enPQ (kernel->rq, toChange, new_priority);
		}
	}
	return toReturn;
}

// Returns a Tuple containing all the processes, first element of the tuple contains the number of elements...
int k_request_process_status (MsgEnv * msg_env_ptr) {
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


