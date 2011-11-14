/*
 * rtx.c
 *
 *  Created on: 2011-11-07
 *      Author: necross
 */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "k_rtx.h"

extern k_RTX * kernel;

/*Helper functions*/
/******************/

//get pcb from pid
PCB * getPCB(int pid){
	PCB * p = SearchQueue(kernel->rq, pid);
	if(!p)
		p = SearchQueue(kernel->bq, pid);
	return p;
}

/*Processes*/
/***********/

//crt-iprocess check local buffer
//if its ready, copy data to share memory and reset local buffer
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



/*Primitives*/
/************/
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

MsgEnv * k_receive_message(){
	//get the process own pcb
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
	PCB * pcb = getPCB(dest_process_id);
	if(!pcb){
		return 0;
	}
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
	return 1;
}

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

