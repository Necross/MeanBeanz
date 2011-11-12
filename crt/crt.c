/*
 * ct.c
 *
 *  Created on: 2011-11-11
 *      Author: necross
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
	int parent_pid, fid, count;
	caddr_t shared_mem_ptr;
	UARTBuffer * output_buff;
	char c;

	sigset(SIGINT,terminator); //If parent requires termination
	sscanf(argv[1], "%d", &parent_pid ); //Obtaining Parents PID
	sscanf(argv[2], "%d", &fid );  //Obtaining Shared File ID
	//Obtaining pointer to shared memory location
	shared_mem_ptr = mmap((caddr_t) 0,   /* Memory Location, 0 lets O/S choose */
			    MAX_BUFFER_SIZE,/* Map 256 bytes */
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
	output_buff->pos = 0;

	do {
		if (output_buff->ok_flag == 1) { // If data is available
			count = 0;
			while (count <= output_buff->size) { // We want to iterate through the entire buffer [Maybe end loop, display of NULL characters?]
					c = output_buff->value[output_buff->pos];
					printf (c);
					output_buff->value[output_buff->pos] = NULL; // Changing the displayed character to NULL
					output_buff->pos++;
					if (output_buff->pos == output_buff->size) { // This is a circular buffer, if the end is reached then it starts at the beginning again
						output_buff->pos = 0;
					}
					count++;
			}
			printf ('\n'); 	// Going to a new line
			output_buff->ok_flag = 0; // Setting flag = 0, implies that stuff was output
			kill (parent_pid, SIGUSR2);
			//Sleeping for a second before checking the shared buffer again
		}
		usleep(100000);
	} while (1);
	return (1);
}
