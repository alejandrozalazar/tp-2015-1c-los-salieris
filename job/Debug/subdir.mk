################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../job.c \
../mapper.c \
../reducer.c 

OBJS += \
./job.o \
./mapper.o \
./reducer.o 

C_DEPS += \
./job.d \
./mapper.d \
./reducer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D"LD_LIBRARY_PATH=/home/utnso/workspace-1c2015/commons/Debug" -I"/home/utnso/workspace-1c2015/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


