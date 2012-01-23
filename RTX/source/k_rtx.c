/*
 * rtx.c
 *
 *  Created on: 2011-11-07
 *      Author: necross
 */

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "k_rtx.h"

extern k_RTX * kernel;
extern UARTBuffer * crt_share_mem;
extern UARTBuffer * kb_share_mem;
void die();

/*Helper functions*/
/******************/

//get pcb from pid
PCB * getPCB(int pid){
	int i;
	for(i=0;i<MAX_PROCESS_NUMBER;i++){
		if(kernel->all_processes[i]->id == pid){
			return kernel->all_processes[i];
		}
	}
	return NULL;
}

void PCB2str(PCB * pcb, char * out){
	//char * out = (char*) malloc(4 * sizeof(char));
	out[0] = '$';
	out[1] = pcb->id;
	out[2] = pcb->priority;
	out[3] = pcb->state;
	//return out;
}

/*Processes*/
/***********/

//crt-iprocess check local buffer
//if its ready, copy data to share memory and reset local buffer
void k_crt_iProcess() {
	kernel->suspended_process = kernel->current_process;
	kernel->current_process = getPCB(CRT_PID);
	MsgEnv * msg_env = k_receive_message();
	if(msg_env != NULL){
		if (msg_env->senderID)
		if(crt_share_mem->ok_flag == 0){
			int i = 0;
			while((crt_share_mem->value[i] = msg_env->msg[i]))
				i++;
			msg_env->type = DISPLAY_ACK;
			k_send_message(msg_env->senderID, msg_env);
			crt_share_mem->ok_flag = 1;
			crt_share_mem->size = i;
		}else{
			//we send the env back to the beginnning of the msg queue
			PCB * crt_pcb = getPCB(CRT_PID);
			msg_env->nextMsgEnv = crt_pcb->msgEnvQueue;
			crt_pcb->msgEnvQueue = msg_env;
		}
	}
	kernel->current_process = kernel->suspended_process;
}


void k_kb_iProcess() {
	kernel->suspended_process = kernel->current_process;
	kernel->current_process = getPCB(KB_PID);

	//k_atomic(on);
	MsgEnv * msg_env = k_receive_message();
	if(msg_env){
		if(kb_share_mem->ok_flag == 1){

			int i = 0;
			while((msg_env->msg[i] = kb_share_mem->value[i]))
				i++;
			msg_env->type = CONSOLE_INPUT;
			k_send_message(msg_env->senderID, msg_env);
			kb_share_mem->size = 0;
			kb_share_mem->ok_flag = 0;
		}else{
			PCB * kb_pcb = getPCB(KB_PID);
			msg_env->nextMsgEnv = kb_pcb->msgEnvQueue->nextMsgEnv;
			kb_pcb->msgEnvQueue->nextMsgEnv = msg_env;
		}
	}
	kernel->current_process = kernel->suspended_process;
	//k_atomic(off);
}

void k_timing_iProcess(){

	if(kernel->current_process == NULL)
		return;

	//PCB * old_process = kernel->current_process;
	//kernel->current_process = getPCB(TIMING_PID);
	k_atomic(on);

	kernel->totalTicks++;
	PCB * timing_pcb = getPCB(TIMING_PID);
	MsgEnv * envPrev = timing_pcb->msgEnvQueue;
	MsgEnv * envCurrent = envPrev;
	int destID = 0;
	while(envCurrent){
		envCurrent->msg[0]--;
			if(envCurrent->msg[0] == 0){
					//dequeue from msg queue
			if(envPrev == envCurrent && envCurrent->nextMsgEnv==NULL){
				timing_pcb->msgEnvQueue = NULL;
			}else if (envPrev == envCurrent)
			{
				timing_pcb->msgEnvQueue = envCurrent->nextMsgEnv;
				envPrev =timing_pcb->msgEnvQueue;
				envCurrent->nextMsgEnv = NULL;
			}else{

				envPrev->nextMsgEnv = envCurrent->nextMsgEnv;
				envCurrent->nextMsgEnv = NULL;
			}
			//send message back to sender process
			envCurrent->type = WAKEUP_CODE;
			destID = envCurrent->senderID;
			envCurrent->senderID = envCurrent->destID;
			k_send_message(destID, envCurrent);
			envCurrent = envPrev->nextMsgEnv;

		}else{
			envPrev = envCurrent;
			envCurrent = envCurrent->nextMsgEnv;
		}
	}
	//k_release_processor();
	k_atomic(off);
	//kernel->current_process = old_process;
}

void k_null_iProcess(){
	while(1){
		k_release_processor();
		usleep(100000);
	}
}

/*Primitives*/
/************/
void k_atomic(a_switch flip) {
	static sigset_t oldmask; //Static set of signals
	sigset_t newmask;
	if (flip == on && kernel->aSwtich == off) {
			sigemptyset(&newmask); //Creating a new signal mask
			sigaddset(&newmask, SIGUSR1);
			sigaddset(&newmask, SIGUSR2);
			sigaddset(&newmask, SIGALRM);
			sigprocmask(SIG_BLOCK, &newmask, &oldmask);
			kernel->aSwtich = on;
	} else if(flip == off && kernel->aSwtich == on) {
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
			kernel->aSwtich = off;
	}
}


MsgEnv * k_receive_message(){
	PCB * pcb = kernel->current_process;

	while(pcb->msgEnvQueue == NULL){
		if(pcb->state != IS_IPROCESS){
			pcb->state = BLOCK_ON_RCV;
			//enPQ(kernel->bq, pcb, pcb->priority);
			k_process_switch();
		}else{
			return NULL;
		}

	}
	MsgEnv * result = pcb->msgEnvQueue;
	pcb->msgEnvQueue = result->nextMsgEnv;
	result->nextMsgEnv = NULL;
	enTraceBuf(kernel->firstReceived, result);
	return result;
}

int k_send_message(int dest_process_id, MsgEnv * msg_envelope){
	PCB * pcb = getPCB(dest_process_id);
	if(!pcb || !msg_envelope){
		return 1;
	}

	//Edit msg info and enqueue to env queue
	msg_envelope->senderID = kernel->current_process->id;
	msg_envelope->destID = dest_process_id;
	MsgEnv * tail = pcb->msgEnvQueue;
	if(tail){
		while(tail->nextMsgEnv)
			tail = tail->nextMsgEnv;
		tail->nextMsgEnv = msg_envelope;
	}else{
		pcb->msgEnvQueue = msg_envelope;
	}


	if(pcb->state == BLOCK_ON_RCV){
		//dePQ(kernel->bq, pcb);
		pcb->state = READY;
		enPQ(kernel->rq, pcb, pcb->priority);
	}

	enTraceBuf(kernel->firstSent, msg_envelope);
	return 0;
}

int k_send_console_chars(MsgEnv * message_envelope ){

	if(!message_envelope){
		return 1;
	}else{
		message_envelope->senderID = kernel->current_process->id;
		int res = k_send_message(CRT_PID, message_envelope);
		if(res == 0)
			//res = kill(getpid(), SIGUSR2);
			k_crt_iProcess ();
		return res;
	}
}

int k_get_console_chars(MsgEnv * message_envelope ){

	if(!message_envelope){
		return 1;
	}else{
		message_envelope->senderID = kernel->current_process->id;
		message_envelope->destID = KB_PID;
		int res = k_send_message(KB_PID, message_envelope);
		return res;
	}
}

MsgEnv * k_request_msg_env (){
	MsgEnv * env = kernel->availMsgEnvQueue->nextMsgEnv;
	while (!env){ // For user processes its messing up here for some reason....
		PCB * pcb = kernel->current_process;
		// Process becomes blocked on envelope_request
		if(pcb->state == IS_IPROCESS){ return NULL;}
			pcb->state = BLOCK_ON_ENV;
			enPQ(kernel->bq, pcb, pcb->priority);
			k_process_switch();
			env = kernel->availMsgEnvQueue->nextMsgEnv;

	}
		kernel->availMsgEnvQueue->nextMsgEnv = kernel->availMsgEnvQueue->nextMsgEnv->nextMsgEnv;
		env->nextMsgEnv = NULL;
		//clear message
		memset(env->msg, 0, MSG_SIZE);
	return env;
}

int k_request_delay( int time_delay, int wakeupCode, MsgEnv * message_envelope ){
	if(message_envelope){
		message_envelope->senderID = kernel->current_process->id;
		message_envelope->destID = TIMING_PID;
		message_envelope->msg[0] = time_delay;
		message_envelope->type = REQUEST_DELAY;
		return k_send_message(TIMING_PID, message_envelope);
	}else{
		return 1;
	}
}


int k_release_msg_env(MsgEnv * msg_env_ptr){
	//Clear fields in envelope
	msg_env_ptr->type = MSG;
	msg_env_ptr->nextMsgEnv = NULL;
	msg_env_ptr->destID = 0;
	msg_env_ptr->senderID = 0;
	int i;


	for (i = 0; i < MSG_SIZE; i++)
	{	msg_env_ptr->msg[i] = 0;}

	//send the message envelope back to available envelope queue
	MsgEnv * tmp_env = kernel->availMsgEnvQueue;
	while(tmp_env->nextMsgEnv)
		tmp_env = tmp_env->nextMsgEnv;
	tmp_env->nextMsgEnv = msg_env_ptr;
	//blk_pcb->state must be BLK_ON_ENV!
	PCB * blk_pcb = dePQ(kernel->bq, NULL);
	if(blk_pcb && blk_pcb->state == BLOCK_ON_ENV){
		blk_pcb->state = READY;
		if(enPQ(kernel->rq, blk_pcb, blk_pcb->priority) == 0)
			return 1;
	}else if(blk_pcb && blk_pcb->state != BLOCK_ON_ENV){
		enPQ(kernel->bq, blk_pcb, blk_pcb->priority);
	}

	return 0;
}

int k_release_processor(){
	PCB * pcb = kernel->current_process;
	if(pcb == NULL)
		return 1;
	if(pcb->id != NULL_PID){
		pcb->state = READY;
		enPQ(kernel->rq, pcb, pcb->priority);
	}
	k_atomic(off);
	k_process_switch();
	return 0;
}

int k_change_priority(int new_priority, int target_process_id) {
	if (new_priority > 3 || new_priority < 0) {
		MsgEnv *output;
		sprintf(output->msg, "KERNEL ERROR: Priority is out of bounds\n");			
		send_console_chars(output);
		return 1;
	} else {
		PCB * pcb = getPCB(target_process_id);
		pcb->priority = new_priority;
	}
	return 0;
}

//#include <execinfo.h>

void k_process_switch(){

	PCB * old_process = kernel->current_process;
	PCB * new_process = dePQ(kernel->rq, NULL);
	if (new_process->id == NULL_PID)
		enPQ(kernel->rq, new_process, new_process->priority);
	if (setjmp(old_process->jbContext) == 0) {
		if(new_process->state != IS_IPROCESS)
			new_process->state = EXECUTING;
		kernel->current_process = new_process;
		k_atomic(off);
		longjmp(new_process->jbContext, 1);
	}
}

void k_terminate(){
	die();
}

int k_request_process_status(MsgEnv * msg_env_ptr){

	char * str = msg_env_ptr->msg;
	const int m_size = 4;
	int i = 0, n = 0;
	char pcb_str[4];

	for(i=0; i<MAX_PROCESS_NUMBER; i++){
		PCB2str(kernel->all_processes[i],pcb_str);
		memcpy(str + i * m_size, pcb_str, m_size);
	}

	str[i * m_size] = '\0';

	return 0;
}

