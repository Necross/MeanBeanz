/*
 * atomic.c
 *
 *  Created on: 2011-11-12
 *      Author: Sohaib
 */

#include <signal.h>
#include <stdio.h>

#include "pcb.h"
#include "k_rtx.h"

k_RTX * kernel; //Need to point to actual kernel structure via "extern" statement when integrated

void k_atomic (a_switch flip) {
	static sigset_t oldmask; 	//Static set of signals
	sigset_t newmask; 			//Newmask will be used to mask and unmask
	if (flip == on) {
			if (kernel->current_process->aCount == 0) { 		//Error check, multiple executions of atomic prohibited
				kernel->current_process->aCount++;			//Turning the atomic indicator in the PCB on
				sigemptyset(&newmask);				//Creating a new signal mask
				sigaddset(&newmask, SIGUSR1);
				sigaddset(&newmask, SIGUSR2);
				sigaddset(&newmask, SIGALRM);
				sigaddset(&newmask, SIGINT);
				sigprocmask(SIG_BLOCK, &newmask, &oldmask);
			}
	} else { 					//The signals are turned back on!
		if (kernel->current_process->aCount == 1) {
			kernel->current_process->aCount--;		//Turning the atomic indicator in the PCB off
			sigemptyset(&newmask);				//Creating a new signal mask
			sigaddset(&newmask, SIGUSR1);
			sigaddset(&newmask, SIGUSR2);
			sigaddset(&newmask, SIGALRM);
			sigaddset(&newmask, SIGINT);
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}
	}
}
