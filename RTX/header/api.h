/*
 * rtx.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef RTX_H_
#define RTX_H_

#include "k_rtx.h"

int send_message( int dest_process_id, MsgEnv * msg_envelope );
MsgEnv * receive_message( );
MsgEnv * request_msg_env( );
int release_msg_env( MsgEnv * msg_env_ptr );
int release_processor( );
int request_process_status( MsgEnv * msg_env_ptr );
void terminate( );
int change_priority(int new_priority, int target_process_id);
int request_delay( int time_delay, int wakeup_code, MsgEnv * message_envelope );
int send_console_chars(MsgEnv * message_envelope );
int get_console_chars(MsgEnv * message_envelope );

#endif /* RTX_H_ */
