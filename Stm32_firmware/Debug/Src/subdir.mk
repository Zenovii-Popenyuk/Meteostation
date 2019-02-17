################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/LCD2004.c \
../Src/ZS-042.c \
../Src/bmp280.c \
../Src/ccsbcs.c \
../Src/dhtxx.c \
../Src/fatfs.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/sd.c \
../Src/spi.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c \
../Src/tim.c \
../Src/us_timing.c \
../Src/usart.c \
../Src/user_diskio.c 

OBJS += \
./Src/LCD2004.o \
./Src/ZS-042.o \
./Src/bmp280.o \
./Src/ccsbcs.o \
./Src/dhtxx.o \
./Src/fatfs.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/sd.o \
./Src/spi.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o \
./Src/tim.o \
./Src/us_timing.o \
./Src/usart.o \
./Src/user_diskio.o 

C_DEPS += \
./Src/LCD2004.d \
./Src/ZS-042.d \
./Src/bmp280.d \
./Src/ccsbcs.d \
./Src/dhtxx.d \
./Src/fatfs.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/sd.d \
./Src/spi.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d \
./Src/tim.d \
./Src/us_timing.d \
./Src/usart.d \
./Src/user_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F411xE -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Inc" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/CMSIS/Include" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Middlewares/Third_Party/FatFs/src"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


