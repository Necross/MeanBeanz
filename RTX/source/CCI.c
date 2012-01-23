/*
 * CCI.c
 *
 *  Created on: Nov 25, 2011
 *      Author: zik
 */

#include <string.h>
#include "api.h"
#include "k_rtx.h"
#include "user_process.h"

extern k_RTX * kernel;

//helper functions

void mapStatus (PCBState state, char * out) {
	if (state == READY) {
		sprintf(out, "READY");
	} else if (state == EXECUTING) {
		sprintf (out, "EXECUTING");
	} else if (state == SUSPENDED) {
		sprintf (out, "SUSPENDED");
	} else if (state == BLOCK_ON_RCV) {
		sprintf (out, "BLOCK ON RECEIVE");
	} else if (state == BLOCK_ON_ENV) {
		sprintf (out, "BLOCK ON ENVELOPE");
	} else if (state == IS_IPROCESS) {
		sprintf (out, "IPROCESS");
	} else if (state == NO_BLK_RCV) {
		sprintf (out, "NO BLOCK RECEIVED");
	} else if (state == NEVER_BLK_PROC) {
		sprintf (out, "NEVER BLOCK PROCESS");
	} else {
		sprintf (out, "ERROR PROCESS");
	}
}

void mapMsgType(MsgType msgtype, char * out){
	if(msgtype == MSG){
		sprintf(out, "Message");
	}else if(msgtype == WAKEUP_CODE){
		sprintf(out, "Wakeup Code");
	}else if(msgtype == REQUEST_DELAY){
		sprintf(out, "Request Delay Message");
	}else if(msgtype == PROCESS_STATUS){
		sprintf(out, "Request Process Status");
	}else if(msgtype == DISPLAY_ACK){
		sprintf(out, "Display Acknowledgment");
	}else if(msgtype == CONSOLE_INPUT){
		sprintf(out, "Console Input Content");
	}else if(msgtype == CHANGE_TIME_TYPE){
		sprintf(out, "Request to change time type");
	}else{
		sprintf(out, "Error Message");
	}
}

int is24h(char * str){
	int i = 0;
	int h = 0, m = 0, s=0;
	for(i=0; i<8; i++)
		if(str[i] == '\0')
			return 0;

	h = (str[0]- '0') * 10 + (str[1] - '0');
	m = (str[3]- '0') * 10 + (str[4] - '0');
	s = (str[6]- '0') * 10 + (str[7] - '0');

	if(h<0 || h>=24 || m <0 || m>=60 || s<0 || s>=60 || str[2]!=':' || str[5]!=':')
		return 0;

	return 1;
}

void setClock(char * time_str){
	kernel->time.hour = (time_str[0] - '0') * 10 + time_str[1] - '0';
	kernel->time.minute = (time_str[3] - '0') * 10 + time_str[4] - '0';
	kernel->time.second = (time_str[6] - '0') * 10 + time_str[7] - '0';
}

void wall_clock_process(){
	MsgEnv * env = request_msg_env();
	MsgEnv * out = request_msg_env();

	while(1){

		request_delay(10, WAKEUP_CODE , env);
		env = receive_message();

		int h = kernel->time.hour, m = kernel->time.minute, s = kernel->time.second;
		s++;
		if(s >= 60){
			s = 0;
			m++;
			if(m >= 60){
				m = 0;
				h++;
				if(h >= 24)
					h = 0;
			}
		}
		kernel->time.hour = h;
		kernel->time.minute = m;
		kernel->time.second = s;


		if(kernel->clock_switch == off){
			continue;
		//	sprintf(out->msg, "%c[s%c[%d;%df%s%c[u", 27,27,1,72,"        ",27);
		}else{
			if (kernel->clock_switch == cool){
				//sprintf(out->msg, "%c[s%c[%d;%df%c[%d;%d;%dm%d%d:%d%d:%d%d%c[u%c[%dm",27,27,1,72,27,1,31,44,h/10,h%10,m/10,m%10,s/10,s%10,27,27,0);
				//sprintf(out->msg, "\27[s\27[1;72f\27[1;31;44m%d%d:%d%d:%d%d\27[u\27[0m",h/10,h%10,m/10,m%10,s/10,s%10);
				sprintf(out->msg, "\033[s\033[0;72H\033[K\033[1;31m%02d\033[1;33m:\033[1;31m%02d\033[1;33m:\033[1;31m%02d\033[u\n\033[1A\033[0m",h,m,s);
			}else{
				sprintf(out->msg,"%d%d:%d%d:%d%d",h/10,h%10,m/10,m%10,s/10,s%10);

				}//sprintf(out->msg, "%02d:%02d:%02d", h, m, s);
		}

		send_console_chars(out);
		out = receive_message();
	}

}

void CCI_Process(){
	MsgEnv * cmd = request_msg_env();
	MsgEnv * output = request_msg_env();
	system("clear");

	while(1){
		sprintf(output->msg, "\033[%d;%dmCCI: \033[%dm\n",1,33,0);
		send_console_chars(output);
		output = receive_message();
		get_console_chars(cmd);
		cmd = receive_message();
		char * text = cmd->msg;

		if(strcmp(text,"s") == 0){
			if (kernel->CCI_flag ==0){
				MsgEnv * toSend = request_msg_env();
				toSend->senderID = CCI_PID;
				send_message(A_PID, toSend);
				sprintf(output->msg, "Starting Process A...\n");
				kernel->CCI_flag =1;
			}else{
				sprintf(output->msg, "You have already started the process\n");
			}
			send_console_chars(output);

		}
		else if(strcmp(text,"help") == 0){	
			sprintf(output->msg, "\nExecutable Commands\n-------------------------------\n'b' to display buffers\n'cd' to display clock\n'std' to display cool clock\n'ct' to hide clock\n't' to terminate\n's' to start process A\n'ps' to Display Process Priority\n'n' to change priority\n-------------------------------\n\nProcesses           PID\n------------------------\nKB                  1\nCRT                 2\nTiming              3\nNull                4\nWall Clock          5\nCCI                 6\nProcesses A         7\nProcesses B         8\nProcesses C         9\n------------------------\n");
		send_console_chars(output);

		}		
		else if(strcmp(text,"ps") == 0){
			request_process_status(output);
			char * processStatus = output->msg;
			char processInfo[1024];
			sprintf(processInfo, "%-20s%-20s%-20s\n", "PID", "Priority",
					"Status");
			int i, pCount = 62;
			char temp[61];
			for (i = 0; i < 60; i++) {
				temp[i] = '-';
			}
			temp[60] = '\n';
			memcpy(processInfo + pCount - 1, temp, 61);
			pCount = pCount + 61;
			i = 0;
			char tmp[30];
			while (processStatus[i] != '\0') {
				mapStatus(processStatus[i + 3], tmp);
				sprintf(temp, "%-20d%-20d%-20s\n", processStatus[i + 1],
						processStatus[i + 2], tmp);
				i = i + 4;
				memcpy(processInfo + pCount - 1, temp, 61);
				pCount = pCount + 61;
			}
			i = 0;
			sprintf(output->msg,"%s", processInfo); // Copying parsed data back into the message envelope
			send_console_chars(output);

		}else if(text[0] == 'c' && text[1] == ' ' && is24h(text + 2)){
			setClock(text+2);
			sprintf(output->msg, "Wall Clock set to 24h format.\n");
			send_console_chars(output);
		}else if(strcmp(text, "cd") == 0){
			kernel->clock_switch = on;
			sprintf(output->msg, "Wall Clock Switched On.\n");
			send_console_chars(output);
		}else if(strcmp(text, "std") == 0){
			kernel->clock_switch = cool;
			sprintf(output->msg, "Cool Wall Clock Switched On.\n");
			send_console_chars(output);
		}else if(strcmp(text, "ct") == 0){
			kernel->clock_switch = off;
			sprintf(output->msg, "Wall Clock Switched Off.\n");
			send_console_chars(output);
		}else if(strcmp(text, "clear") == 0){
			system("clear");
			continue;
		}else if(strcmp(text, "b") == 0){
			printf ("Requesting Trace Buffer Status\n");
						getTraceBuf(cmd);

						text = cmd->msg;
						//cmd = receive_message();
						char * out = output->msg;
						char str[30], temp[80];
						int i=0, j=0;
						//temp is for every new line(nicely formatted by sprintf, fixed length),
						//and we append it to output string "out" using memcpy
						//sprintf(out,"%-20s%-20s%-20s%-20s\n","Destination PID","Sender PID","Message Type","Time Stamp");
						sprintf (out,"%-20s%-20s%-20s%-20s\n","\nsenderID","destID","type","time stamp(secound)\n");
						while(i<32){

							//mapMsgType(text[i*4+2] - '0', str);
							if (text[i*4+2] == MSG)
								sprintf(str, "MSG");
							else if (text[i*4+2]== WAKEUP_CODE)
								sprintf(str, "WAKEUP_CODE");
							else if (text[i*4+2]== REQUEST_DELAY)
								sprintf(str, "REQUEST_DELAY");
							else if (text[i*4+2]== PROCESS_STATUS)
								sprintf(str, "PROCESS_STATUS");
							else if (text[i*4+2]== DISPLAY_ACK)
								sprintf(str, "DISPLAY_ACK");
							else if (text[i*4+2]== CONSOLE_INPUT)
								sprintf(str, "CONSOLE_INPUT");
						    else if (text[i*4+2]== CHANGE_TIME_TYPE)
						    	sprintf(str, "CHANGE_TIME_TYPE");
							if (i==16 && j==0){
								sprintf (temp,"%-80s\n","\nsenderID             destID            type               time stamp(secound)\n");
								j++;
							}else
							{

								sprintf(temp,"%-20d%-20d%-20s%-20ld\n",text[i*4],text[i*4+1],str,kernel->firstSent->tbn[text[i*4+3]].timeStamp);
								i++;
							}
								//cmd = receive_message();
							memcpy(out+ 81*i, temp, 81);
						//memcpy(out + 81 * i -1, temp, 81);
						}

						//cmd = receive_message();
						out[81*(i+1)] = '\0';
						send_console_chars(output);


		}else if(strcmp(text, "t") == 0){
			printf ("Terminating RTX....");
			terminate();
		}else if(text[0] == 'n' && text[1] == ' ' && text[2] >= '0' && text[2] < '3' && text[3] == ' ' && text[4] > '0' && text[4] <= '9' && text[5] == '\0'&& text[4]!='4' && text[4]!='6'){
			change_priority(text[2] - '0',text[4] - '0');
			sprintf(output->msg, "Priority of Process %c change to %c", text[4], text[2]);
			send_console_chars(output);
/*
		else if (text[4]=='4'){		
		sprintf(output->msg, "Cannot change the priority of the null process!\n");
		send_console_chars(output);		
		}		
		else if (text[4]=='6'){		
		sprintf(output->msg, "Cannot change the priority of the CCI process!\n");
		send_console_chars(output);		
		}*/
		}else{
			sprintf(output->msg, "Wrong input!\n");
			send_console_chars(output);
		}
		output = receive_message();
	}
}
