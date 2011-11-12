/*
 * main.c
 *
 *  Created on: Nov 9, 2011
 *      Author: Necross
 */
<<<<<<< HEAD

//CODE ATOMIC FUNCTION


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
//#include "k_rtx.h"
#include "buffer.h"
#include "global.h"


// globals
UARTBuffer  * input_buffer, * output_buffer;		// pointer to structure that is the shared memory
int in_pid;				// pid of keyboard child process
void * shared_input_ptr, * shared_output_ptr;
int fidInput, status, fidOutput;		//used to create the shared memory

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char * inputFile = "keyboardTest";  //the name of the shared_memory file for input
char * outputFile = "outputTest";   //Output file
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//**************************************************************************
// routine to clean up things before terminating main program
// This stuff must be cleaned up or we have child processes and shared
//	memory hanging around after the main process terminates

void cleanup()
{
	// terminate child process(es)
	kill(in_pid,SIGINT);
	// remove shared memory segment and do some standard error checks
	status = munmap(shared_input_ptr, MAX_BUFFER_SIZE);
    if (status == -1){
      printf("Bad munmap during cleanup\n");
    }
	// close the temporary mmap file
    status = close(fidInput);
    if (status == -1){
      printf("Bad close of temporary mmap file during cleanup\n");
    };
	// unlink (i.e. delete) the temporary mmap file
    status = unlink(inputFile);
    if (status == -1){
      printf("Bad unlink during cleanup.\n");
    }

  //Output Buffer Cleanup
	kill(in_pid,SIGINT);
	// remove shared memory segment and do some standard error checks
	status = munmap(shared_output_ptr, MAX_BUFFER_SIZE);
    if (status == -1){
      printf("Bad munmap during cleanup\n");
    }
	// close the temporary mmap file
    status = close(fidOutput);
    if (status == -1){
      printf("Bad close of temporary mmap file during cleanup\n");
    };
	// unlink (i.e. delete) the temporary mmap file
    status = unlink(outputFile);
    if (status == -1){
      printf("Bad unlink during cleanup.\n");
    }
}


//**************************************************************************
// routine to call before exiting
// This routine gets called when certain signals occur

void die(int signal)
{
	cleanup();
	printf( "\n\nSignal Received.   Leaving demo ...\n" );
	exit(0);
}

// kbd_handler
// Called by signal SIGUSR1 from keyboard reader process

//Coding CRT for testing purposes
void crt_handler (int signum) {
	input_buffer->ok_flag = 0;  // tell child that the buffer has been emptied
}


//(count <= input_buffer->size) &&
void kbd_handler(int signum) {
	//UARTBuffer command;
	int count = 0;
	// copy input buffer
	if (input_buffer->value [0] != '\0')
	{
		//Copy onto the output buffer
		while ((input_buffer->value[count] != '\0') ) {
			output_buffer->value[count] = input_buffer->value[count];
			count++;
		}
		output_buffer->value[count] = '\0';
		//strcpy(output_buffer->value ,input_buffer->value );
	     // we should parse the input string and execute the command given,
	    //  but for now we just echo the input
	    //
	   // printf("Keyboard input was: %s\n",command.value );
	    output_buffer->ok_flag = 1; //telling output buffer that data is available for output
	}

}


//**************************************************************************
int main()
{


	// catch signals so we can clean up everything before exitting
	// signals defined in /usr/include/signal.h
	// e.g. when we recieved an interrupt signal SIGINT, call die()
	sigset(SIGINT,die);		// catch kill signals
	sigset(SIGBUS,die);		// catch bus errors
	sigset(SIGHUP,die);
	sigset(SIGILL,die);		// illegal instruction
	sigset(SIGQUIT,die);
	sigset(SIGABRT,die);
	sigset(SIGTERM,die);
	sigset(SIGSEGV,die);	// catch segmentation faults

	sigset(SIGUSR1,kbd_handler);
	sigset(SIGUSR2,crt_handler);



  /* Create a new mmap file for read/write access with permissions restricted  to owner rwx access only */
  fidInput = open(inputFile, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
  if (fidInput < 0){
	  printf("Bad Open of mmap file <%s>\n", inputFile);
	  exit(0);
  };

  /* Create a new mmap file for read/write access with permissions restricted  to owner rwx access only */
  fidOutput = open(outputFile, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755 );
  if (fidOutput < 0){
	  printf("Bad Open of mmap file <%s>\n", outputFile);
	  exit(0);
  };


  // make the file the same size as the buffer
  status = ftruncate(fidInput, MAX_BUFFER_SIZE );
  if (status){
      printf("Failed to ftruncate the file <%s>, status = %d\n", inputFile, status );
      exit(0);
  }

  // make the file the same size as the buffer
  status = ftruncate(fidOutput, MAX_BUFFER_SIZE );
  if (status){
      printf("Failed to ftruncate the file <%s>, status = %d\n", outputFile, status );
      exit(0);
  }

	// pass parent's process id and the file id to child
	char childarg1[20], childarg2[20]; // arguments to pass to child process(es)
	int mypid = getpid();			// get current process pid

	sprintf(childarg1, "%d", mypid); // convert to string to pass to child
    sprintf(childarg2, "%d", fidInput);   // convert the file identifier
=======
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

k_RTX * kernel;

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


//**************************************************************************
// routine to call before exiting
// This routine gets called when certain signals occur

void die(int signal)
{
	cleanup();
	free(kernel);
	printf( "\n\nSignal Received. Leaving demo ...\n" );
	exit(0);
}

// kbd_handler
// Called by signal SIGUSR1 from keyboard reader process

//Coding CRT for testing purposes
void k_crt_iProcess() {
	//if(kernel->crt_buf->ok_flag == 1)
		kernel->kb_buf->ok_flag = 0;
}

void k_kb_iProcess() {
	if(kernel->kb_buf->value[0] != '\0'){
		int count = 0;
		while(kernel->kb_buf->value[count]){
			kernel->crt_buf->value[count] = kernel->kb_buf->value[count];
			count++;
		}
		kernel->crt_buf->value[count] = '\0';
		kernel->crt_buf->ok_flag = 1;
	}
}

int process_initialization(){
	//Initialize member variables
	kernel = (k_RTX *)malloc(sizeof(k_RTX));
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
	}*/

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

	kernel->kb_mfile = "kb_map";
	kernel->crt_mfile = "crt_map";

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
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695


	// create the keyboard reader process
	// fork() creates a second process identical to the current process,
<<<<<<< HEAD
	// except that the "parent" process has in_pid = new process's ID,
	// while the new (child) process has in_pid = 0.
	// After fork(), we do execl() to start the actual child program.
	// (see the fork and execl man pages for more info)

	in_pid = fork();
	if (in_pid == 0)	// is this the child process ?
	{
		execl("./keyboard", "keyboard", childarg1, childarg2, (char *)0);
=======
	// except that the "parent" process has kernel->kb_pid = new process's ID,
	// while the new (child) process has kernel->kb_pid = 0.
	// After fork(), we do execl() to start the actual child program.
	// (see the fork and execl man pages for more info)

	kernel->kb_pid = fork();
	if (kernel->kb_pid == 0) // is this the child process ?
	{
		execl("./KB", "keyboard", childarg1, childarg2, (char *)0);
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
		// should never reach here
		fprintf(stderr,"demo: can't exec keyboard, errno %d\n",errno);
		cleanup();
		exit(1);
	};

	// pass parent's process id and the file id to child
	char childarg3[20], childarg4[20]; // arguments to pass to child process(es)
	sprintf(childarg3, "%d", mypid); // convert to string to pass to child
<<<<<<< HEAD
    sprintf(childarg4, "%d", fidOutput);   // convert the file identifier
	in_pid = fork();
	if (in_pid == 0)	// is this the child process ?
	{
		execl("./crt", "crt", childarg1, childarg2, (char *)0);
		// should never reach here
		fprintf(stderr,"demo: can't exec crt , errno %d\n",errno);
		cleanup();
		exit(1);
	};

	// the parent process continues executing here

	// sleep for a second to give the child process time to start
	sleep(1);

	// allocate a shared memory region using mmap
	// the child process also uses this region

    shared_input_ptr = mmap((void *) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fidInput,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
    if (shared_input_ptr == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do cleanup and terminate
    };

    input_buffer = (UARTBuffer *) shared_input_ptr;   // pointer to shared memory
    	  // we can now use 'input_buffer' as a standard C pointer to access
    	  // the created shared memory segment


    //SHARED OUTPUT BUFFER
    shared_output_ptr = mmap((void *) 0,   /* Memory location, 0 lets O/S choose */
		    MAX_BUFFER_SIZE,              /* How many bytes to mmap */
		    PROT_READ | PROT_WRITE, /* Read and write permissions */
		    MAP_SHARED,    /* Accessible by another process */
		    fidOutput,           /* the file associated with mmap */
		    (off_t) 0);    /* Offset within a page frame */
    if (shared_output_ptr == MAP_FAILED){
      printf("Parent's memory map has failed, about to quit!\n");
	  die(0);  // do cleanup and terminate
    };
    output_buffer = (UARTBuffer *) shared_output_ptr;   // pointer to shared memory
    	  // we can now use 'input_buffer' as a standard C pointer to access
    	  // the created shared memory segment

    output_buffer->ok_flag = 0;

	// now start doing whatever work you are supposed to do
	// in this case, do nothing; only the keyboard handler will do work
	input_buffer->ok_flag = 0;
	printf("\nType something followed by end-of-line and it will be echoed\n\n");
	do {

	} while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
=======
	sprintf(childarg4, "%d", kernel->crt_fid); // convert the file identifier
	kernel->crt_pid = fork();
	if (kernel->crt_pid == 0) // is this the child process ?
	{
		execl("./CRT", "crt", childarg1, childarg2, (char *)0);
		// should never reach here
		fprintf(stderr,"demo: can't exec crt.c , errno %d\n",errno);
		cleanup();
		exit(1);
	};

	// the parent process continues executing here

	// sleep for a second to give the child process time to start
	sleep(1);

	// allocate a shared memory region using mmap
	// the child process also uses this region

	kernel->kb_mem = mmap((void *) 0, /* Memory location, 0 lets O/S choose */
			MAX_BUFFER_SIZE, /* How many bytes to mmap */
			PROT_READ | PROT_WRITE, /* Read and write permissions */
			MAP_SHARED, /* Accessible by another process */
			kernel->kb_fid, /* the file associated with mmap */
			(off_t) 0); /* Offset within a page frame */
	if (kernel->kb_mem == MAP_FAILED){
	    printf("Parent's memory map has failed, about to quit!\n");
	    die(0); // do cleanup and terminate
	};

	kernel->kb_buf = (UARTBuffer *) kernel->kb_mem; // pointer to shared memory
	// we can now use 'kernel->kb_buf' as a standard C pointer to access
	// the created shared memory segment


	//SHARED OUTPUT BUFFER
	kernel->crt_mem = mmap((void *) 0, /* Memory location, 0 lets O/S choose */
			MAX_BUFFER_SIZE, /* How many bytes to mmap */
			PROT_READ | PROT_WRITE, /* Read and write permissions */
			MAP_SHARED, /* Accessible by another process */
			kernel->crt_fid, /* the file associated with mmap */
			(off_t) 0); /* Offset within a page frame */
	if (kernel->crt_mem == MAP_FAILED){
	      printf("Parent's memory map has failed, about to quit!\n");
	      die(0); // do cleanup and terminate
	};
	kernel->crt_buf = (UARTBuffer *) kernel->crt_mem; // pointer to shared memory
	// we can now use 'kernel->kb_buf' as a standard C pointer to access
	// the created shared memory segment

	kernel->crt_buf->ok_flag = 0;

	// now start doing whatever work you are supposed to do
	// in this case, do nothing; only the keyboard handler will do work
	kernel->kb_buf->ok_flag = 0;
	printf("\nType something followed by end-of-line and it will be echoed\n\n");
	while (1);

	// should never reach here, but in case we do, clean up after ourselves
	cleanup();
	exit(1);
}

int main () {
	if(!process_initialization())
		return 1;
	else
		return 0;
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
}
