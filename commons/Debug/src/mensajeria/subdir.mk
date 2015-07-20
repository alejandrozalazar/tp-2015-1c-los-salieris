################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mensajeria/comunicacion.c \
../src/mensajeria/sockets.c 

OBJS += \
./src/mensajeria/comunicacion.o \
./src/mensajeria/sockets.o 

C_DEPS += \
./src/mensajeria/comunicacion.d \
./src/mensajeria/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/mensajeria/%.o: ../src/mensajeria/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


