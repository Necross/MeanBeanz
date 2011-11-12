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

//Information for the operation
typedef void* OpInfo;

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
	//Available Envelope Queue(array)
	MsgEnv * availMsgEnvQueue;

	//Total pulse/ticks timed
	long totalTicks;
	//Flag for wall clock
	bool wallclock_enable;

	UARTBuffer * kb_buf;
	UARTBuffer * crt_buf;
	//PID of Keyboard & CRT iProcess
	int kb_pid;
	int crt_pid;
	//Memory Mapping Pointer for input/output buffer
	void * kb_mem;
	void * crt_mem;
	//File ID for input/output buffer
	int kb_fid;
	int crt_fid;
	//File Name for input/output buffer
	char * kb_mfile;
	char * crt_mfile;

	//Trace Buffer Arrays
	TraceBuffer * firstSent;
	TraceBuffer * firstReceived;
};

//Initializing RTX
void k_rtxInit(k_RTX * rtx, int numPCB, InitTable * iT, int numMsgEnv);

//Quit RTX
void k_rtxDestroy(k_RTX * rtx);

/*Message & Envelope*/

//Request a free message envelope
MsgEnv * k_request_msg_env();

//Receive a message envelope
MsgEnv * k_receive_msg();

//Send message to a dest_process
int k_send_messsage(int dest_process_id, MsgEnv * msg_envelope);

//Send a delayed message
int k_request_delay( int time_delay, int wakeupCode, MsgEnv * message_envelope );

//Release a message envelope and make it available
int k_release_msg_env(MsgEnv * msg_env_ptr);

/*Process Scheduling*/

//Release and free processor
int k_release_processor();

//Get a status of all processes
int k_request_process_status(MsgEnv * msg_env_ptr);

//Process Scheduler; discussed later
int k_scheduler(PCB * proc, Op PO, OpInfo info);

//Process Switch from current_process to a new process
int process_switch(PCB* newProc);

//enqueue process in readyQ
int rpq_enqueue(PCB * proc);

//dequeue process from readyQ
PCB * rpq_dequeue();

int k_change_priority (int new_priority, int target_process_id);

/*iProcesses*/

//Keyboard & CRT iProcesses
void k_kb_iProcess();
void k_crt_iProcess();

//Null iProcess
int k_null_iProcess();

//Timing Service Process
int k_timing_iProcess();

//Wall clock process
void wall_clock_process();
void k_setClock(TimeType time);
TimeType k_getClock();

//Atomicity
void k_atomic(a_switch);
void atomic(a_switch);

//Input-Output
int k_send_console_chars(MsgEnv * message_envelope );
int k_get_console_chars(MsgEnv * message_envelope );

//Trace
int getTraceBuf(MsgEnv * msgEnv);
int setTraceBufSent(MsgEnv *msgEnv);
int setTraceBufReceived(MsgEnv * msgEnv);
int enTraceBuf(TraceBuffer * traceBuf, MsgEnv * msgEnv);

//Terminate
int k_terminate();

PCB * getPCB(int pid);

#endif /* K_RTX_H_ */
