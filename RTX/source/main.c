/*
 * main.c
 *
 *  Created on: Nov 9, 2011
 *      Author: Necross
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "k_rtx.h"
#include "buffer.h"
#include "global.h"
#include "user_process.h"

//RTX Globals
k_RTX * kernel;
UARTBuffer * crt_share_mem;
UARTBuffer * kb_share_mem;

void die();
void deallowcate_all();



/*=============================================================================
 * 							EXIT ROUTINES
==============================================================================*/

void die()
{
	deallowcate_all();
	printf( "\n\nSignal Received. Terminating ...\n" );
	exit(0);
}


void deallowcate_all()
{
	// terminate child process(es)
	kill(kernel->kb_pid,SIGINT);

	// remove shared memory segment and do some standard error checks
	if (munmap(kernel->kb_mem, MAX_BUFFER_SIZE) == -1)
      printf("Bad kb munmap during deallowcate_all\n");

	// close the temporary mmap file
    if (close(kernel->kb_fid) == -1)
      printf("Bad close of temporary kb mmap file during deallowcate_all\n");

    // unlink (i.e. delete) the temporary mmap file
    if (unlink(kernel->kb_mfile) == -1)
      printf("Bad unlink kb_mfile during deallowcate_all.\n");

    //Output Buffer deallowcate_all
    kill(kernel->crt_pid,SIGINT);

    // remove shared memory segment and do some standard error checks
    if (munmap(kernel->crt_mem, MAX_BUFFER_SIZE) == -1)
      printf("Bad crt munmap during deallowcate_all\n");

    // close the temporary mmap file
    if (close(kernel->crt_fid) == -1)
      printf("Bad close of temporary crt mmap file during deallowcate_all\n");

    // unlink (i.e. delete) the temporary mmap file
    if (unlink(kernel->crt_mfile) == -1)
      printf("Bad unlink crt_mfile during deallowcate_all.\n");

  /*  if(!PCBQueueDestroy(&(kernel->rq)))
    	printf("Unable to destroy ready queue!\n");
    if(!PCBQueueDestroy(&(kernel->bq)))
    	printf("Unable to destroy block queue!\n");

    int i;
    //Deallowcate all processes
    for(i=0; i<MAX_PROCESS_NUMBER; i++)
    	free(kernel->all_processes[i]);

    //Deallowcate message envelopes
    for(i=0; i<AVAIL_MSG_ENV_SIZE; i++)
    	free(&(kernel->availMsgEnvQueue[i]));

    free(kernel);*/
}

/*===========================================================================
 * 					INITIALIZATION FUNCTIONS
============================================================================*/

void init_signals () {


	sigset(SIGBUS,die);
	sigset(SIGHUP,die);
	sigset(SIGILL,die);
	sigset(SIGQUIT,die);
	sigset(SIGABRT,die);
	sigset(SIGTERM,die);
	sigset(SIGSEGV,die);
	sigset(SIGINT,die);
	sigset(SIGUSR1,k_kb_iProcess);
	sigset(SIGUSR2,k_crt_iProcess);
	sigset(SIGALRM,k_timing_iProcess);

	ualarm(100000, 100000);
}


void init_helper_proc () {

	memcpy(kernel->kb_mfile, "kb_map", 7);
	memcpy(kernel->crt_mfile, "crt_map", 8);

	 /* Create a new mmap file for read/write access with permissions restricted to owner rwx access only */
	kernel->kb_fid = open(kernel->kb_mfile, O_RDWR | O_CREAT, (mode_t) 0755 );
	if (kernel->kb_fid < 0){
		printf("Bad Open of mmap file <%s>\n", kernel->kb_mfile);
		exit(0);
	};

	/* Create a new mmap file for read/write access with permissions restricted to owner rwx access only */
	kernel->crt_fid = open(kernel->crt_mfile, O_RDWR | O_CREAT, (mode_t) 0755 );
	if (kernel->crt_fid < 0){
		printf("Bad Open of mmap file <%s>\n", kernel->crt_mfile);
		exit(0);
	};


	// make the file the same size as the buffer
	if (ftruncate(kernel->kb_fid, MAX_BUFFER_SIZE ) == -1){
	    printf("Failed to ftruncate the file <%s>\n", kernel->kb_mfile);
	    exit(0);
	}

	// make the file the same size as the buffer
	if (ftruncate(kernel->crt_fid, MAX_BUFFER_SIZE ) == -1){
	    printf("Failed to ftruncate the file <%s>\n", kernel->crt_mfile);
	    exit(0);
	}

	// pass parent's process id and the file id to child
	int mypid = getpid();
	char childarg1[20], childarg2[20]; // arguments to pass to child process(es)
	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
	sprintf(childarg2, "%d", kernel->kb_fid); // convert the file identifier


	// create the keyboard reader process
	// fork() creates a second process identical to the current process,
	// except that the "parent" process has in_pid = new process's ID,
	// while the new (child) process has in_pid = 0.
	// After fork(), we do execl() to start the actual child program.
	// (see the fork and execl man pages for more info)
	kernel->kb_pid = fork();
	if (kernel->kb_pid == 0) // is this the child process ?
	{
		execl("./KB", "keyboard", childarg1, childarg2, (char *)0);
		// should never reach here
		fprintf(stderr,"Can't exec keyboard, errno %d\n",errno);
		deallowcate_all();
		exit(1);
	};


	//CHECK DATA TYPES

	// pass parent's process id and the file id to child
	char childarg3[20], childarg4[20]; // arguments to pass to child process(es)
	sprintf(childarg3, "%d", mypid); // convert to string to pass to child
    sprintf(childarg4, "%d", kernel->crt_fid);   // convert the file identifier
	kernel->crt_pid = fork();
	if (kernel->crt_pid == 0)	// is this the child process ?
	{
		execl("./CRT", "crt", childarg3, childarg4, (char *)0);
		// should never reach here
		fprintf(stderr,"demo: can't execute crt , errno %d\n",errno);
		deallowcate_all();
		exit(1);
	};

	// the parent process continues executing here sleep for a second to give the child process time to start
	sleep(1);

	// allocate a shared memory region using mmap
	// the child process also uses this region
	caddr_t kb_mem;
	 kb_mem = mmap((caddr_t) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    kernel->kb_fid,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
	// kernel->kb_mem = kb_share_mem;
	 if (kb_mem == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do deallowcate_all and terminate
    };


    //SHARED OUTPUT BUFFER
    caddr_t crt_mem;
    crt_mem = mmap((caddr_t) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    kernel->crt_fid,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */

    //kernel->crt_mem = crt_share_mem;
    if (crt_mem == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do deallowcate_all and terminate
    };

    crt_share_mem = (UARTBuffer *)(crt_mem);
    kb_share_mem = (UARTBuffer *)(kb_mem);

    //memset(kb_share_mem,0,sizeof(UARTBuffer));
    //memset(crt_share_mem,0,sizeof(UARTBuffer));
   // UARTBuffer * kb_share_mem = (UARTBuffer *)(kernel->kb_mem);
    //UARTBuffer * crt_share_mem = (UARTBuffer *)(kernel->crt_mem);

}

int traceBufferInit()
{
		kernel->firstReceived = malloc(sizeof(TraceBuffer));
		kernel->firstReceived->tail = 0;
		kernel->firstSent = malloc(sizeof(TraceBuffer));
		kernel->firstSent->tail = 0;
		return 0;
}

int init_rtx(){

	kernel = (k_RTX *)malloc(sizeof(k_RTX));

	if(kernel == NULL){
		printf("Fail to initialize kernel. Aborting...");
		exit(0);
	}
	kernel->aSwtich = off;
	kernel->clock_switch = off;
	kernel->CCI_flag = 0;

	//CRT & KB Initialization
	if(PCBQueueInit(&(kernel->rq))==0 || PCBQueueInit(&(kernel->bq))==0)
		return 0;

	//Envelope Memory Allocation
	kernel->availMsgEnvQueue = (MsgEnv *)malloc((AVAIL_MSG_ENV_SIZE+1) * sizeof(MsgEnv));
	int i;
	for(i=0; i<AVAIL_MSG_ENV_SIZE; i++){
		kernel->availMsgEnvQueue[i].nextMsgEnv = &(kernel->availMsgEnvQueue[i+1]);
		kernel->availMsgEnvQueue[i].msg = (Msg)malloc(MSG_SIZE * sizeof(char));
	}
	kernel->availMsgEnvQueue[119].nextMsgEnv->nextMsgEnv = NULL;
	kernel->availMsgEnvQueue[119].nextMsgEnv = NULL;

	// Code to test if last element was NULL
	/*int x = 0;
	MsgEnv * temp = kernel->availMsgEnvQueue;
	while (temp) {
		printf ("%d\n", x);
		temp = temp->nextMsgEnv;
		x++;
	}*/

	//Initialize PCB of iProcesses
	PCB * kb_pcb = NULL, * crt_pcb = NULL, * timing_pcb = NULL, * null_pcb = NULL;

	if(pcbInit(&kb_pcb,KB_PID,3,(void *)k_kb_iProcess,IS_IPROCESS) == 0 ||
		pcbInit(&crt_pcb,CRT_PID,3,(void *)k_crt_iProcess,IS_IPROCESS) == 0 ||
		pcbInit(&timing_pcb, TIMING_PID,3,(void *)k_timing_iProcess, IS_IPROCESS) == 0 ||
		pcbInit(&null_pcb, NULL_PID, 3, (void *)k_null_iProcess, IS_IPROCESS) == 0)
		return 0;


	if(enPQ(kernel->rq, null_pcb, null_pcb->priority) == 0)
		return 0;

	kernel->all_processes[0] = kb_pcb;
	kernel->all_processes[1] = crt_pcb;
	kernel->all_processes[2] = timing_pcb;
	kernel->all_processes[3] = null_pcb;
	traceBufferInit();


	return 1;
}

//TO ADD
//Initiliaze wall clock process & null process!
int init_user_proc(){

	//Make initialization table
	InitTable it[5];
	if(!initTableInit(&(it[0]),CCI_PID,0,USER_PROCESS,MAX_STACK_SIZE,(void *)CCI_Process) ||
		!initTableInit(&(it[1]),A_PID,1,USER_PROCESS,MAX_STACK_SIZE,(void *)ProcessA) ||
		!initTableInit(&(it[2]),B_PID,1,USER_PROCESS,MAX_STACK_SIZE,(void *)ProcessB) ||
		!initTableInit(&(it[3]),C_PID,1,USER_PROCESS,MAX_STACK_SIZE,(void *)ProcessC) ||
		!initTableInit(&(it[4]),WALL_CLOCK_PID,0,USER_PROCESS,MAX_STACK_SIZE,(void *)wall_clock_process))
		return 0;


	//Initialize PCB
	//Might be problematic here since C is really dumb about 2d array
	PCB * usr_pcb[5];
	int i;
	for(i=0; i<5; i++){
		if(pcbInit(usr_pcb + i,it[i].id,it[i].priority,it[i].pc,READY)){
			if(!enPQ(kernel->rq,usr_pcb[i],usr_pcb[i]->priority))
				return 0;
		}else{
			return 0;
		}
		kernel->all_processes[4+i] = usr_pcb[i];
	}

	jmp_buf context;
	PCB *p = NULL;
	char * jmpsp = NULL;
	for (i = 4; i < 10; i++) {
		p = getPCB(i);
		//p->sp = (char *)malloc(MAX_STACK_SIZE);
		p->sp = p->sp + MAX_STACK_SIZE - 4;
		if (!setjmp(context)) {

			jmpsp = p->sp;
			__asm__ ("movl %0,%%esp" :"=m" (jmpsp));

			if (setjmp(p->jbContext) == 0) {
				longjmp(context, 1);
			} else {

				void (*start)();
				start = (void *)kernel->current_process->pc;
				start();

			}
		}
	}

	//Start Wall Clock
	PCB * wall_clock = getPCB(WALL_CLOCK_PID);
	dePQ(kernel->rq, wall_clock);
	kernel->current_process = wall_clock;
	longjmp(wall_clock->jbContext, 1);

	/*
	//Start CCI
	PCB * cci = dePQ(kernel->rq, NULL); //this should be CCI
	kernel->current_process = cci;
	longjmp(cci->jbContext, 1);*/

	/**********************
	 This part is for partial
	 **********************
	 **********************
	//Initialize PCBs
	PCB * user_proc = NULL;
	if(pcbInit(&user_proc,USER_PROCESS_PID_START + 0,0,0, READY)){
		if(!enPQ(kernel->rq,user_proc,user_proc->priority))
			return 0;
	}else{
		return 0;
	}

	//Run user_proc

	kernel->current_process = user_proc;
	kernel->current_process->state = NEVER_BLK_PROC;
	processP();
	***********************/

	return 1;
}

//**************************************************************************
// routine to call before exiting
// This routine gets called when certain signals occur

//Coding CRT for testing purposes

int main () {

	//Initialize kernel
	if(init_rtx() == 0){
		printf("fail to initialize kernel! Aborting");
		die();
	}

	//Initialize signal and keyboard/crt Process
	init_signals ();
	init_helper_proc ();

	//Start User-process
	if(init_user_proc() == 0)
		printf("fail to run user process!");

	//Should never get here unless user process not properly running
	die();

	//Should never never get here
	exit(1);
}
