################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Pheripheral/Src/dht11.c 

OBJS += \
./Pheripheral/Src/dht11.o 

C_DEPS += \
./Pheripheral/Src/dht11.d 


# Each subdirectory must supply rules for building sources it contributes
Pheripheral/Src/%.o: ../Pheripheral/Src/%.c Pheripheral/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Pheripheral-2f-Src

clean-Pheripheral-2f-Src:
	-$(RM) ./Pheripheral/Src/dht11.d ./Pheripheral/Src/dht11.o

.PHONY: clean-Pheripheral-2f-Src

