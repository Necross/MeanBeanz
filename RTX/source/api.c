/*
 * api.c
 *
 *  Created on: Nov 12, 2011
 *      Author: zik
 */

#include "api.h"


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
