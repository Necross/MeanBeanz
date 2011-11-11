/*
 * rtx.c
 *
 *  Created on: 2011-11-07
 *      Author: necross
 */

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "k_rtx.h"

k_RTX * kernel;

PCB * getPCB(int pid){
	PCB * pcb;
	pcb = SearchQueue(kernel->rq, pid);
	if(!pcb){
		pcb = SearchQueue(kernel->bq, pid);
	}
	return pcb;
}

MsgEnv * k_receive_msg(){
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

int k_send_messsage(int dest_process_id, MsgEnv * msg_envelope){
	PCB * pcb = getPCB(dest_process_id);
	if(!pcb){
		return 0;
	}
	msg_envelope->senderID = kernel->current_process;
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
	if(message_envelope->msg.str){
		if(!k_send_message(KB_PID,message_envelope)){
			return 0;
		}
		kill(getpid(),SIGUSR2);
		message_envelope = k_receive_message();
		return k_send_message(message_envelope->senderID, message_envelope);
	}else{
		k_send_message(message_envelope->senderID, message_envelope);
		return 0;
	}
}

int k_get_console_chars(MsgEnv * message_envelope ){
	while(kernel->kb_buf->ok_flag == 0)
		sleep(100);
	if(!(message_envelope->msg.str->value = strcpy(kernel->kb_buf, message_envelope->msg.str->value))){
		return 0;
	}else{
		kernel->kb_buf->ok_flag = 0;
		kernel->kb_buf->size = strlen(kernel->kb_buf);
		int result = k_send_message(message_envelope->senderID, message_envelope);
		return result;
	}
}
