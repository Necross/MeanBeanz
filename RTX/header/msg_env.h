/*
 * msg.h
 *
 *  Created on: Nov 7, 2011
 *      Author: zik
 */

#ifndef MSG_H_
#define MSG_H_

#include "global.h"
#include "buffer.h"
#include "pcb.h"

//Message Type
typedef union msg{
	int value;
	UARTBuffer * buf;
	PCBState state;
	long int time;
} Msg;

typedef struct msg_env{
	//message envelope is a linked list itself
	struct msg_env * nextMsgEnv;
	int senderID;
	int destID;
	MsgType type;
	int priority;
	Msg msg;
} MsgEnv;

int msgEnvInit(MsgEnv * msgEnv);
int msgEnvDestroy(MsgEnv * msgEnv);
void eraseMsg(MsgEnv * msgEnv);

#endif /* MSG_H_ */
