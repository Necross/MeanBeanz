################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/init_table.c \
../source/main.c \
../source/msg_env.c \
../source/pcb.c \
../source/queue.c \
../source/rtx.c 

OBJS += \
./source/init_table.o \
./source/main.o \
./source/msg_env.o \
./source/pcb.o \
./source/queue.o \
./source/rtx.o 

C_DEPS += \
./source/init_table.d \
./source/main.d \
./source/msg_env.d \
./source/pcb.d \
./source/queue.d \
./source/rtx.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/zik/Documents/Project/RTX/header" -O3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


