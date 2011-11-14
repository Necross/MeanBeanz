CC=gcc
IFLAG=-I./RTX/header
CFLAG=-c -Wall
RTX_SOURCE=api.c init_table.c k_rtx.c msg.c pcb.c queue.c user_process.c main.c
RTX_OBJ=api.o init_table.o k_rtx.o msg.o pcb.o queue.o user_process.o main.o
KB_SOURCE=kb.c
KB_OBJ=kb.o
CRT_SOURCE=crt.c
CRT_OBJ=crt.o

all: iRTX-G29 KB CRT clean

iRTX-G29: $(RTX_OBJ)
	$(CC) -o iRTX-G29 $(RTX_OBJ)
api.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/api.c
init_table.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/init_table.c
k_rtx.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/k_rtx.c
msg.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/msg.c
pcb.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/pcb.c
queue.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/queue.c
user_process.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/user_process.c
main.o:
	$(CC) $(FLAG) $(IFLAG) -c RTX/source/main.c

KB: $(KB_OBJ)
	$(CC) -o KB $(KB_OBJ)
kb.o:
	$(CC) $(FLAG) $(IFLAG) -c KB_HELPER/kb.c

CRT: $(CRT_OBJ)
	$(CC) -o CRT $(CRT_OBJ)
crt.o:
	$(CC) $(FLAG) $(IFLAG) -c CRT_HELPER/crt.c

clean:
	rm -f *.o
