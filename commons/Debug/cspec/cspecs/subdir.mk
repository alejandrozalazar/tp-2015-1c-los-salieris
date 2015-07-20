################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cspec/cspecs/cspec.c 

OBJS += \
./cspec/cspecs/cspec.o 

C_DEPS += \
./cspec/cspecs/cspec.d 


# Each subdirectory must supply rules for building sources it contributes
cspec/cspecs/%.o: ../cspec/cspecs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


