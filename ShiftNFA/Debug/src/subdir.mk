################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/NFA.cpp \
../src/ShiftTNFA.cpp 

CPP_DEPS += \
./src/NFA.d \
./src/ShiftTNFA.d 

OBJS += \
./src/NFA.o \
./src/ShiftTNFA.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++2a -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/NFA.d ./src/NFA.o ./src/ShiftTNFA.d ./src/ShiftTNFA.o

.PHONY: clean-src

