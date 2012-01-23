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
#define MAX_STACK_SIZE 8192*2
#define MAX_TRACE_SIZE 16
#define MAX_QUEUE_SIZE 255
#define MAX_BUFFER_SIZE 2048
#define PRIORITY_QUEUE_SIZE 3
#define AVAIL_MSG_ENV_SIZE 120
#define MSG_SIZE 2048

/*PIDs*/
#define RESERVED 0
#define KB_PID 1
#define CRT_PID 2
#define TIMING_PID 3
#define NULL_PID 4
#define WALL_CLOCK_PID 5
#define CCI_PID 6
#define A_PID 7
#define B_PID 8
#define C_PID 9

#define MAX_PROCESS_NUMBER 9

//The list of states of process:
//READY: ready to execute; EXECUTING: running; SUSPENDED: execution suspended;
//INTERRUPTED: interrupted by signal/envelope; BLOCK_ON_RCV: blocked on receiving signal/message
//BLOCK_ON_ENV: blocked on no available envelope;
//IS_IPROCESS: it is an iProcess
typedef enum pcb_state{
	READY, EXECUTING, SUSPENDED,
	BLOCK_ON_RCV, BLOCK_ON_ENV,IS_IPROCESS,
	NO_BLK_RCV, NEVER_BLK_PROC
} PCBState;

typedef enum process_type{
	KERNEL_IPROCESS, USER_PROCESS
} ProcessType;

//Message Type
typedef enum msg_type{
	MSG, WAKEUP_CODE, REQUEST_DELAY, PROCESS_STATUS,
	DISPLAY_ACK, CONSOLE_INPUT,
	CHANGE_TIME_TYPE, COUNT_REPORT
} MsgType;

typedef enum atomic_switch{
	off,on,cool
} a_switch;

//Operation to be done with a process
typedef enum proc_operation{
	BLOCK_PROCESS, UNBLOCK_PROCESS, SUSPEND_PROCESS,
	CHANGE_PRIORITY, ADD_PROCESS, ADD_TO_READY_QUEUE,
	SWITCH_TO_READY_PROCESS, START_RTX
} Op;

typedef struct char_buf UARTBuffer;
typedef struct trace_buf TraceBuffer;
typedef struct tbn TraceBufferNode;
typedef char * Msg;
typedef struct msg_env MsgEnv;
typedef struct pcb_record InitTable;
typedef struct time_field TimeType;
typedef struct k_rtx k_RTX;
typedef struct pcb PCB;

#endif /* GLOBAL_H_ */
