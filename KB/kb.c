/*
 * kb.c
 *
 *  Created on: Nov 12, 2011
 *      Author: zik
 */

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "global.h"
#include "buffer.h"

int buffer_index;

//Clean up before exiting
	//RTX takes care of this
void terminator(int signal)
{
	exit(0);
}

//Arguments sent int [parent_pid fileName_for_shared_memory]
int main (int argc, char * argv[])
{
	int parent_pid, fid;
	void * shared_mem_ptr;
	UARTBuffer * input_buff;
	char c;


	sigset(SIGINT,terminator); //If parent requires termination
	sscanf(argv[1], "%d", &parent_pid ); //Obtaining Parents PID
	sscanf(argv[2], "%d", &fid );  //Obtaining Shared File ID
	//Obtaining pointer to shared memory location
	shared_mem_ptr = mmap((void *) 0,   // Memory Location, 0 lets O/S choose
		    MAX_BUFFER_SIZE, // Map 256 bytes
		    PROT_READ | PROT_WRITE, // Read and write permissions
		    MAP_SHARED,    // Accessible by another process
		    fid,           // which file is associated with mmap
		    (off_t) 0);    // Offset in page frame
   //If unable to map shared memory
	if (shared_mem_ptr == MAP_FAILED){
		printf("Memory map has failed, Keyboard helper process is aborting!\n");
		terminator(0);
    }
	input_buff = (UARTBuffer *) shared_mem_ptr; //Pointing the input buffer to the obtained shared memory
	buffer_index = 0; //Initializing indexes
	input_buff->ok_flag = 0; //ok_flag = 0, means no data in the buffer
	do
	{
		c = getchar();
		if ( c != '\n' ) {	//If the input is not a carriage
					if( buffer_index < MAX_BUFFER_SIZE ) { //Buffer size is not exceeded (256 characters)
						input_buff->value[buffer_index++] = c;
					}
				} else {
					input_buff->value[buffer_index] = '\0';
					input_buff->ok_flag = 1;  //set ready status bit
					kill(parent_pid,SIGUSR1); //Send signal to parent. SIGUSR1 is mapped to the keyboard iProcess
					buffer_index = 0;  // for now, just restart ???
					while( input_buff->ok_flag == 1) //Sleep until input is not read in by the Kernel
						usleep(100000);
				}
	}while(1);  //an infinite loop - exit when parent signals us
}
