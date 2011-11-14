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


//RTX Globals
k_RTX * kernel;

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

    if(!PCBQueueDestroy(&(kernel->rq)))
    	printf("Unable to destroy ready queue!\n");
    if(!PCBQueueDestroy(&(kernel->bq)))
    	printf("Unable to destroy block queue!\n");

    //Deallowcate message envelopes
    free(kernel->availMsgEnvQueue);
    free(kernel);
}

/*===========================================================================
 * 					INITIALIZATION FUNCTIONS
============================================================================*/

void ini_signals () {

	sigset(SIGINT,die);
	sigset(SIGBUS,die);
	sigset(SIGHUP,die);
	sigset(SIGILL,die);
	sigset(SIGQUIT,die);
	sigset(SIGABRT,die);
	sigset(SIGTERM,die);
	sigset(SIGSEGV,die);
	sigset(SIGUSR1,k_kb_iProcess);
	sigset(SIGUSR2,k_crt_iProcess);

}


void ini_helper_proc () {

	kernel->kb_mfile = "kb_map";
	kernel->crt_mfile = "crt_map";
	//ini_help_Proc ();

	 /* Create a new mmap file for read/write access with permissions restricted to owner rwx access only */
	kernel->kb_fid = open(kernel->kb_mfile, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
	if (kernel->kb_fid < 0){
		printf("Bad Open of mmap file <%s>\n", kernel->kb_mfile);
		exit(0);
	};

	/* Create a new mmap file for read/write access with permissions restricted to owner rwx access only */
	kernel->crt_fid = open(kernel->crt_mfile, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
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

    kernel->kb_mem = mmap((void *) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    kernel->kb_fid,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
    if (kernel->kb_mem == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do deallowcate_all and terminate
    };

    kernel->kb_buf = (UARTBuffer *)malloc(sizeof(UARTBuffer));   // pointer to shared memory
    	  // we can now use 'input_buffer' as a standard C pointer to access
    	  // the created shared memory segment

    //SHARED OUTPUT BUFFER
    kernel->crt_mem = mmap((void *) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    kernel->crt_fid,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
    if (kernel->crt_mem == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do deallowcate_all and terminate
    };
    kernel->crt_buf = (UARTBuffer *)malloc(sizeof(UARTBuffer));   // pointer to shared memory
    	  // we can now use 'input_buffer' as a standard C pointer to access
    	  // the created shared memory segment

	kernel->crt_buf->ok_flag = 0;
	kernel->kb_buf->ok_flag = 0;
}

int init_rtx(){

	kernel = (k_RTX *)malloc(sizeof(k_RTX));
	if(kernel == NULL){
		printf("Fail to initialize kernel. Aborting...");
		exit(0);
	}

	//Data Structure Initialization
	if(PCBQueueInit(&(kernel->rq))==0)
		return 0;
	if(PCBQueueInit(&(kernel->bq))==0)
		return 0;
	kernel->availMsgEnvQueue = (MsgEnv *)malloc(AVAIL_MSG_ENV_SIZE * sizeof(MsgEnv));
	int i;
	for(i=0; i<AVAIL_MSG_ENV_SIZE-1; i++)
		kernel->availMsgEnvQueue[i].nextMsgEnv = &(kernel->availMsgEnvQueue[i+1]);

	//Initialize PCB
	PCB * kb_pcb = NULL, * crt_pcb = NULL;
	if(pcbInit(&kb_pcb,KB_PID,2,0,0,IS_IPROCESS) && pcbInit(&crt_pcb,CRT_PID,2,0,0,IS_IPROCESS)){
		if(enPQ(kernel->rq,kb_pcb,kb_pcb->priority)==0 || enPQ(kernel->rq,crt_pcb,crt_pcb->priority)==0)
			return 0;
	}else{
		return 0;
	}

	return 1;
}

int run_user_proc(){

	PCB * user_proc = NULL;
	if(pcbInit(&user_proc,USER_PROCESS_PID_START + 0,0,0,0, READY)){
		if(!enPQ(kernel->rq,user_proc,user_proc->priority))
			return 0;
	}else{
		return 0;
	}

	//Run user_proc

	kernel->current_process = user_proc;
	kernel->current_process->state = NEVER_BLK_PROC;
	processP();

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
	ini_signals ();
	ini_helper_proc ();

	//Start User-process
	if(run_user_proc())
		printf("fail to run user process!");

	//Should never get here unless user process not properly running
	die();

	//Should never never get here
	exit(1);
}
