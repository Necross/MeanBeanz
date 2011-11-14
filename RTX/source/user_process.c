/*
 * user_process.c
 *
 *  Created on: Nov 12, 2011
 *      Author: zik
 */

#include "user_process.h"

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
