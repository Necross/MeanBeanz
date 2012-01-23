/*
 * k_rtx.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef K_RTX_H_
#define K_RTX_H_

//include all headers
#include "global.h"
#include "msg.h"
#include "pcb.h"
#include "init_table.h"
#include "queue.h"
#include "buffer.h"


//24-hour time format
struct time_field{
	int hour; int minute; int second;
};

//Kernel structure of RTX
struct k_rtx {
	//Ready Queue; queue of process ready to be executed
	PCBQueue * rq;
	//Block Process Queue
	PCBQueue * bq;

	//Current context running process
	PCB * current_process;
	PCB * suspended_process;
	PCB * all_processes[MAX_PROCESS_NUMBER];
	//Available Envelope Queue(array)
	MsgEnv * availMsgEnvQueue;

	a_switch aSwtich;

	//Total pulse/ticks timed
	long totalTicks;
	//Flag for wall clock
	a_switch clock_switch;
	TimeType time;

	//UARTBuffer * kb_buf;
	//UARTBuffer * crt_buf;
	//PID of Keyboard & CRT iProcess
	int kb_pid;
	int crt_pid;
	int CCI_flag;
	//Memory Mapping Pointer for input/output buffer
	void * kb_mem;
	void * crt_mem;
	//File ID for input/output buffer
	int kb_fid;
	int crt_fid;
	//File Name for input/output buffer
	char kb_mfile[10];
	char crt_mfile[10];

	//Trace Buffer Arrays
	TraceBuffer * firstSent;
	TraceBuffer * firstReceived;
};

/*Message & Envelope*/

//Request a free message envelope
MsgEnv * k_request_msg_env();

//Receive a message envelope
MsgEnv * k_receive_message();

//Send message to a dest_process
int k_send_message(int dest_process_id, MsgEnv * msg_envelope);

//Send a delayed message
int k_request_delay( int time_delay, int wakeupCode, MsgEnv * message_envelope );

//Release a message envelope and make it available
int k_release_msg_env(MsgEnv * msg_env_ptr);

/*Process Scheduling*/

//Release and free processor
int k_release_processor();

//Get a status of all processes
int k_request_process_status(MsgEnv * msg_env_ptr);

//Process Switch from current_process to a new process
void k_process_switch();

int k_change_priority(int new_priority, int target_process_id);

/*iProcesses*/

//Keyboard & CRT iProcesses
void k_kb_iProcess();
void k_crt_iProcess();

//Null iProcess
void k_null_iProcess();

//Timing Service Process
void k_timing_iProcess();

//Wall clock process
void wall_clock_process();
void setClock(char * time);

//Console
int k_get_console_chars(MsgEnv * message_envelope );
int k_send_console_chars(MsgEnv * message_envelope );

//Atomicity
void k_atomic(a_switch);

//Trace
int getTraceBuf(MsgEnv * msgEnv);
int setTraceBufSent(MsgEnv *msgEnv);
int setTraceBufReceived(MsgEnv * msgEnv);
int enTraceBuf(TraceBuffer * traceBuf, MsgEnv * msgEnv);

//Terminate
void k_terminate();

PCB * getPCB(int pid);

#endif /* K_RTX_H_ */
