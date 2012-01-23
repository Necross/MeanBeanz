/*
 * user_process.c
 *
 *  Created on: Nov 12, 2011
 *      Author: zik
 */

#include <string.h>
#include <limits.h>
#include "user_process.h"

extern k_RTX * kernel;
MsgEnv * local_queue;

void LocalEnqueue(MsgEnv * msg_env);
MsgEnv * LocalDequeue();

void processP( void ){

	const int tWait = 500000;
	MsgEnv * env;
	env = request_msg_env(); //rcv loop wait time in usec, approx value
	while (1) {
		get_console_chars(env); //keyboard input
		env = receive_message(); //Process does not block
		while (env == NULL) {
			usleep(tWait);
			env = receive_message();
		}
		send_console_chars(env);
		env = receive_message(); //CRT output, wait for acknowledgement
		while (env == NULL) {
			usleep(tWait);
			env = receive_message();
		}
	}
}

void ProcessA(){
	MsgEnv * message = NULL;
	static int num = 0;
	message = receive_message();
	release_msg_env(message);
	do{
		message = request_msg_env();
		message->type = COUNT_REPORT;
		message->msg[0] =num;
		message->senderID = B_PID;
		send_message(B_PID, message);
		num = (num < INT_MAX) ? (num+1) : 0;
		release_processor();
	}while(1);
}

void ProcessB(){
	MsgEnv * message = NULL;
	do{
		message = receive_message();
		message->senderID = B_PID;
		send_message(C_PID, message);
	}while(1);
}

void ProcessC(){
	MsgEnv * message = NULL;
	while(1){
		message = LocalDequeue();
		if((message == NULL) && ((message = receive_message()) == NULL))
			terminate();
		if(message->type == COUNT_REPORT &&  (message->msg[0]%20 == 0)){
				message->type = MSG;
				sprintf(message->msg, "Process C\n");
				send_console_chars(message);
				message = receive_message();
				while (message->type != DISPLAY_ACK)
				{
					LocalEnqueue(message);
					message = receive_message();
				}
				message->senderID = C_PID;
				request_delay(100, WAKEUP_CODE, message);

				message = receive_message();
				while (message->type != WAKEUP_CODE) {

					LocalEnqueue(message);
					message = receive_message();
				}

			}
				release_msg_env(message);
				release_processor();
		}

}




void LocalEnqueue(MsgEnv * msg_env){
	if(local_queue == NULL){
		local_queue = msg_env;
	}else{
		MsgEnv * current = local_queue;
		while(current->nextMsgEnv){
			current = current->nextMsgEnv;
		}
		current->nextMsgEnv = msg_env;
	}
}

MsgEnv * LocalDequeue(){
	if(local_queue == NULL){
		return NULL;
	}else if(local_queue->nextMsgEnv == NULL){
		MsgEnv * temp = local_queue;
		local_queue = NULL;
		return temp;
	}else{
		MsgEnv * current = local_queue;
		while(current->nextMsgEnv->nextMsgEnv){
			current = current->nextMsgEnv;
		}
		MsgEnv * temp = current->nextMsgEnv;
		current->nextMsgEnv = NULL;
		return temp;
	}
}

