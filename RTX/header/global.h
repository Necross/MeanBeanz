/*
 * global.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdbool.h>

//SIZE CONSTANTS
#define MAX_STACK_SIZE 255
#define MAX_QUEUE_SIZE 255
#define MAX_BUFFER_SIZE 255
#define PRIORITY_QUEUE_SIZE 3
#define AVAIL_MSG_ENV_SIZE 255
#define MSG_SIZE

//PIDs
#define KB_PID 1
#define CRT_PID 2
#define TIME_PID 3
#define WALL_CLOCK_PID 4
#define TERMINATOR_PID 5
#define USER_PROCESS_PID_START 6

//Program Counter
typedef void* PC;
//Stack Pointer
typedef void* SP;
//Message Type
typedef enum msg_type{
	WAKEUP_CODE, PROCESS_STATUS,
	DISPLAY_ACK, CONSOLE_INPUT, REQUEST_INPUT, REQUEST_OUTPUT,
	DISPLAY_TIME, REQUEST_TIME, CHANGE_TIME_TYPE
} MsgType;

#endif /* GLOBAL_H_ */
