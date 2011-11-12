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
#include "k_rtx.h"
#include "buffer.h"
#include "global.h"


//RTX Globals
k_RTX * kernel;
int status;		//used to create the shared memory



/*=============================================================================
 * 							EXIT ROUTINES
==============================================================================*/

void die(int signal)
{
	cleanup();
	free(kernel);
	printf( "\n\nSignal Received. Leaving demo ...\n" );
	exit(0);
}


void cleanup()
{
	// terminate child process(es)
	kill(kernel->kb_pid,SIGINT);

	// remove shared memory segment and do some standard error checks
	if (munmap(kernel->kb_mem, MAX_BUFFER_SIZE) == -1)
      printf("Bad munmap during cleanup\n");

	// close the temporary mmap file
    if (close(kernel->kb_fid) == -1)
      printf("Bad close of temporary mmap file during cleanup\n");

    // unlink (i.e. delete) the temporary mmap file
    if (unlink(kernel->kb_mfile) == -1)
      printf("Bad unlink during cleanup.\n");

    //Output Buffer Cleanup
    kill(kernel->crt_pid,SIGINT);

    // remove shared memory segment and do some standard error checks
    if (munmap(kernel->crt_mem, MAX_BUFFER_SIZE) == -1)
      printf("Bad munmap during cleanup\n");

    // close the temporary mmap file
    if (close(kernel->crt_fid) == -1)
      printf("Bad close of temporary mmap file during cleanup\n");

    // unlink (i.e. delete) the temporary mmap file
    if (unlink(kernel->crt_mfile) == -1)
      printf("Bad unlink during cleanup.\n");
}

/*===========================================================================
 * 					INITIALIZATION FUNCTIONS
============================================================================*/

void ini_Signals () {
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


void ini_helper_Proc () {

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
		fprintf(stderr,"demo: can't exec keyboard, errno %d\n",errno);
		cleanup();
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
		execl("./crt", "crt", childarg3, childarg4, (char *)0);
		// should never reach here
		fprintf(stderr,"demo: can't exec crt , errno %d\n",errno);
		cleanup();
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
	  die(0);  // do cleanup and terminate
    };

    kernel->kb_buf = (UARTBuffer *) kernel->kb_mem;   // pointer to shared memory
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
	  die(0);  // do cleanup and terminate
    };
    kernel->crt_buf = (UARTBuffer *) kernel->crt_mem;   // pointer to shared memory
    	  // we can now use 'input_buffer' as a standard C pointer to access
    	  // the created shared memory segment
    kernel->crt_buf->ok_flag = 0;

	// now start doing whatever work you are supposed to do
	// in this case, do nothing; only the keyboard handler will do work
	kernel->kb_buf->ok_flag = 0;
	printf("\nType something followed by end-of-line and it will be echoed\n\n");
	do {

	} while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
}



//**************************************************************************
// routine to call before exiting
// This routine gets called when certain signals occur

//Coding CRT for testing purposes
void k_crt_iProcess() {
	//if(kernel->crt_buf->ok_flag == 1)
		printf ("Got here son");
		kernel->kb_buf->ok_flag = 0;
}

void k_kb_iProcess() {
	if(kernel->kb_buf->value[0] != '\0'){
		int count = 0;
		while(kernel->kb_buf->value[count] != '\0'){
			kernel->crt_buf->value[count] = kernel->kb_buf->value[count];
			count++;
		}
		kernel->crt_buf->value[count] = '\0';
		kernel->crt_buf->ok_flag = 1;
	}
}
/*
int process_initialization(){
	//Initialize member variables
	/*
	if(!PCBQueueInit(kernel->rq))return 0;
	if(!PCBQueueInit(kernel->bq))return 0;
	//kernel->availMsgEnvQueue = (MsgEnv *)malloc(AVAIL_MSG_ENV_SIZE * sizeof(MsgEnv));
	//for(int i=0; i<AVAIL_MSG_ENV_SIZE; i++)
	//	if(!msgEnvInit(kernel->availMsgEnvQueue[i]))
	//		return 0;
	//Starting processes
	if(!k_kb_iProcess()){
		return 0;
	}else{
		PCB * kbp;
		if(pcbInit(kbp,KB_PID,2,0,0,IS_IPROCESS)){
			return 0;
		}else{
			if(enPQ(kernel->rq,kbp,kbp->priority))
				return 0;
		}
	}

	if(!k_crt_iProcess()){
		return 0;
	}else{
		PCB * crtp;
		if(pcbInit(crtp,CRT_PID,2,0,0,IS_IPROCESS)){
			return 0;
		}else{
			if(enPQ(kernel->rq,crtp,crtp->priority))
				return 0;
		}
	}

	// now start doing whatever work you are supposed to do
	// in this case, do nothing; only the keyboard handler will do work
	kernel->kb_buf->ok_flag = 0;
	printf("\nType something followed by end-of-line and it will be echoed\n\n");
	while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
} */

int main () {

	kernel = (k_RTX *)malloc(sizeof(k_RTX));
	ini_Signals ();
	ini_helper_Proc ();
	kernel->kb_buf->ok_flag = 0;
	printf("\nType something followed by end-of-line and it will be echoed\n\n");
	while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
	/*if(!process_initialization())
		return 1;
	else
		return 0;*/
}
