/*
 * api.c
 *
 *  Created on: 2011-11-12
 *      Author: necross
 */

//USER PROCESS API --- Incomplete


#include "api.h"


int send_message( int dest_process_id, MsgEnv * msg_envelope ){
	k_atomic (on);
	k_send_message (dest_process_id, msg_envelope);
	k_atomic (off);
	return (1);
}


int release_msg_env( MsgEnv * msg_env_ptr ){
	k_atomic (on);
	k_release_msg_env (msg_env_ptr);
	k_atomic (off);
	return (1);
}

int release_processor( ){
	k_atomic (on);
	k_release_processor ();
	k_atomic (off);
	return (1);
}


int request_process_status( MsgEnv * msg_env_ptr ){
	k_atomic (on);
	k_request_process_status (msg_env_ptr);
	k_atomic (off);
	return (1);
}

int terminate( ){
	k_atomic (on);
	k_terminate ();
	k_atomic (off);
}

int change_priority(int new_priority, int target_process_id){
	k_atomic (on);
	k_change_priority (new_priority, target_process_id);
	k_atomic (off);
	return (1);
}


int request_delay( int time_delay, int wakeup_code, MsgEnv * msg_env ){
	k_atomic (on);
	k_request_delay (time_delay, wakeup_code, msg_env);
	k_atomic (off);
	return (1);
}


//Actual working ones needed for Initial Implementation

int send_console_chars(MsgEnv * msg_env) {
	k_atomic (on);
	k_send_console_chars (msg_env);
	k_atomic (off);
	return (1);
}

int get_console_chars(MsgEnv * msg_env ) {
	k_atomic (on);
	k_get_console_chars (msg_env);
	k_atomic (off);
	return (1);
}

MsgEnv * receive_message( ){
	k_atomic (on);
	k_receive_message ();
	k_atomic (off);
	return (1);
}

MsgEnv * request_msg_env( ) {
	k_atomic (on);
	k_request_msg_env ();
	k_atomic (off);
	return (1);
}
