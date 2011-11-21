################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/api.c \
../source/init_table.c \
../source/k_rtx.c \
../source/main.c \
../source/msg.c \
../source/pcb.c \
../source/queue.c \
../source/user_process.c 

OBJS += \
./source/api.o \
./source/init_table.o \
./source/k_rtx.o \
./source/main.o \
./source/msg.o \
./source/pcb.o \
./source/queue.o \
./source/user_process.o 

C_DEPS += \
./source/api.d \
./source/init_table.d \
./source/k_rtx.d \
./source/main.d \
./source/msg.d \
./source/pcb.d \
./source/queue.d \
./source/user_process.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/necross/Documents/MeanBeanz/RTX/header" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


