################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/init_table.c \
<<<<<<< HEAD
../source/k_atomic.c \
=======
../source/k_rtx.c \
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
../source/main.c \
../source/msg.c \
../source/pcb.c \
../source/queue.c 

OBJS += \
./source/init_table.o \
<<<<<<< HEAD
./source/k_atomic.o \
=======
./source/k_rtx.o \
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
./source/main.o \
./source/msg.o \
./source/pcb.o \
./source/queue.o 

C_DEPS += \
./source/init_table.d \
<<<<<<< HEAD
./source/k_atomic.d \
=======
./source/k_rtx.d \
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
./source/main.d \
./source/msg.d \
./source/pcb.d \
./source/queue.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
<<<<<<< HEAD
	gcc -I/home/necross/Documents/MeanBeanz/RTX/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
=======
	gcc -I"/home/zik/Documents/MeanBeanz/RTX/header" -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
>>>>>>> 4f6f6026dd5ef0e2af44aa762facef2e5819a695
	@echo 'Finished building: $<'
	@echo ' '


