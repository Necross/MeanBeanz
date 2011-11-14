/*
 * crt.c
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
	int parent_pid, fid, count;
	void * shared_mem_ptr;
	UARTBuffer * output_buff;

	sigset(SIGINT,terminator); //If parent requires termination
	sscanf(argv[1], "%d", &parent_pid ); //Obtaining Parents PID
	sscanf(argv[2], "%d", &fid );  //Obtaining Shared File ID
	//Obtaining pointer to shared memory location
	shared_mem_ptr = mmap((void *) 0,   /* Memory Location, 0 lets O/S choose */
			    MAX_BUFFER_SIZE, /* Map 256 bytes */
			    PROT_READ | PROT_WRITE, /* Read and write permissions */
			    MAP_SHARED,    /* Accessible by another process */
			    fid,           /* which file is associated with mmap */
			    (off_t) 0);    /* Offset in page frame */
	//If unable to map shared memory
	if (shared_mem_ptr == MAP_FAILED){
		printf("Memory map has failed, crt.c helper process is aborting!\n");
		terminator(0);
    }

	output_buff = (UARTBuffer *) shared_mem_ptr; // Pointing the input buffer to the obtained shared memory
	buffer_index = 0; // Initializing indexes
	output_buff->ok_flag = 0; // ok_flag = 0, means no data in the buffer
	output_buff->size = MAX_BUFFER_SIZE;

	do {
		while(output_buff->ok_flag == 0) { //Sleep until input is not read in by the Kernel
			usleep(100000);
		}
		if (output_buff->ok_flag == 1) { // If data is available
			count = 0;
			while ( (count <= output_buff->size) && (output_buff->value[count] != '\0') ) {
					printf ("%c",output_buff->value[count]);
					count++;
			}
			if(output_buff->value[0] != '\0')
				printf ("\n");
			output_buff->ok_flag = 0; // Setting flag = 0, implies that stuff was output
			kill (parent_pid, SIGUSR2);
			//Sleeping for a second before checking the shared buffer again
		}
	} while (1);
}
