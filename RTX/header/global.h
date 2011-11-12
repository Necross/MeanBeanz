/*
 * global.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/*SIZE CONSTANTS*/
#define MAX_STACK_SIZE 255
#define MAX_QUEUE_SIZE 255
#define MAX_BUFFER_SIZE 255
#define PRIORITY_QUEUE_SIZE 3
#define AVAIL_MSG_ENV_SIZE 20
#define MSG_SIZE 255
//Maximum amount of characters allowed in the input/output buffer
#define MAXCHAR 255


/*PIDs*/
#define KB_PID 1
#define CRT_PID 2
#define TIME_PID 3
#define WALL_CLOCK_PID 4
#define TERMINATOR_PID 5
#define USER_PROCESS_PID_START 6

//The list of states of process:
//READY: ready to execute; EXECUTING: running; SUSPENDED: execution suspended;
//INTERRUPTED: interrupted by signal/envelope; BLOCK_ON_RCV: blocked on receiving signal/message
//BLOCK_ON_ENV: blocked on no available envelope;
//IS_IPROCESS: it is an iProcess
typedef enum pcb_state{
	READY, EXECUTING, SUSPENDED, INTERRUPTED,
	BLOCK_ON_RCV, BLOCK_ON_ENV,
	IS_IPROCESS, NO_BLK_RCV
} PCBState;

//Message Type
typedef enum msg_type{
	WAKEUP_CODE, PROCESS_STATUS,
	DISPLAY_ACK, CONSOLE_INPUT, REQUEST_INPUT, REQUEST_OUTPUT,
	DISPLAY_TIME, REQUEST_TIME, CHANGE_TIME_TYPE
} MsgType;

typedef enum atomic_switch{
	on,off
} a_switch;
//Operation to be done with a process
typedef enum proc_operation{
	BLOCK_PROCESS, UNBLOCK_PROCESS, SUSPEND_PROCESS,
	CHANGE_PRIORITY, ADD_PROCESS, ADD_TO_READY_QUEUE,
	SWITCH_TO_READY_PROCESS, START_RTX
} Op;

typedef struct char_buf UARTBuffer;
typedef struct trace_buf TraceBuffer;
typedef union msg Msg;
typedef struct msg_env MsgEnv;
typedef struct pcb_record InitTable;
typedef struct time_field TimeType;
typedef struct k_rtx k_RTX;
typedef struct pcb PCB;

#endif /* GLOBAL_H_ */
