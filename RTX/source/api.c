/*
 * api.c
 *
 *  Created on: Nov 12, 2011
 *      Author: zik
 */

#include "api.h"

int send_message( int dest_process_id, MsgEnv * msg_envelope ){
	k_atomic(on);
	int result = k_send_message(dest_process_id, msg_envelope);
	k_atomic(off);
	return result;
}

int send_console_chars(MsgEnv * msg_env) {
	k_atomic (on);
	int result = k_send_console_chars (msg_env);
	k_atomic (off);
	return result;
}

int get_console_chars(MsgEnv * msg_env ) {
	k_atomic (on);
	int result = k_get_console_chars (msg_env);
	k_atomic (off);
	return result;
}

MsgEnv * receive_message( ){
	k_atomic (on);
	MsgEnv * result = k_receive_message ();
	k_atomic (off);
	return result;
}


MsgEnv * request_msg_env( ) {
	k_atomic (on);
	MsgEnv * result = k_request_msg_env ();
	k_atomic (off);
	return result;
}

int release_msg_env( MsgEnv * msg_env_ptr ){
	k_atomic(on);
	int result = k_release_msg_env(msg_env_ptr);
	k_atomic(off);
	return result;
}

int release_processor(){
	k_atomic(on);
	int result = k_release_processor();
	k_atomic(off);
	return result;
}

int request_delay( int time_delay, int wakeup_code, MsgEnv * message_envelope){
	k_atomic(on);
	int result = k_request_delay(time_delay, wakeup_code, message_envelope);
	k_atomic(off);
	return result;
}

int request_process_status( MsgEnv * msg_env_ptr ){
	k_atomic(on);
	int result = k_request_process_status(msg_env_ptr);
	k_atomic(off);
	return result;
}

int change_priority(int new_priority, int target_process_id){
	k_atomic(on);
	int result = k_change_priority(new_priority, target_process_id);
	k_atomic(off);
	return result;
}

void terminate( ){
	k_terminate();
}
