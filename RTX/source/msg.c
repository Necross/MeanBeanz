/*
 * msg.c
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#include "msg.h"

int msgEnvInit(MsgEnv ** msgEnv){
	*msgEnv = (MsgEnv *)malloc(sizeof(MsgEnv));
	if(*msgEnv)
		return 1;
	else
		return 0;
}

int msgEnvDestroy(MsgEnv ** msgEnv){
	free((*msgEnv)->msg);
	free(*msgEnv);
	if(*msgEnv)
		return 0;
	else
		return 1;
}
