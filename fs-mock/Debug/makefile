################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: fs-mock

dependents:
	-cd /home/utnso/workspace-1c2015/commons/Debug && $(MAKE) all

# Tool invocations
fs-mock: $(OBJS) $(USER_OBJS) /home/utnso/workspace-1c2015/commons/Debug/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace-1c2015/commons/Debug" -o "fs-mock" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) fs-mock
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/workspace-1c2015/commons/Debug/libcommons.so:

-include ../makefile.targets
