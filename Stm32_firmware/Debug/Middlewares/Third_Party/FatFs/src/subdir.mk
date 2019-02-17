################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FatFs/src/diskio.c \
../Middlewares/Third_Party/FatFs/src/ff.c \
../Middlewares/Third_Party/FatFs/src/ff_gen_drv.c 

OBJS += \
./Middlewares/Third_Party/FatFs/src/diskio.o \
./Middlewares/Third_Party/FatFs/src/ff.o \
./Middlewares/Third_Party/FatFs/src/ff_gen_drv.o 

C_DEPS += \
./Middlewares/Third_Party/FatFs/src/diskio.d \
./Middlewares/Third_Party/FatFs/src/ff.d \
./Middlewares/Third_Party/FatFs/src/ff_gen_drv.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FatFs/src/%.o: ../Middlewares/Third_Party/FatFs/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F411xE -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Inc" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Drivers/CMSIS/Include" -I"D:/Projects/Second_year/POC/Folder_for_cubemix/Meteostation/Middlewares/Third_Party/FatFs/src"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


