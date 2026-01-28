################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/system_ch32v20x.c 

C_DEPS += \
./User/ch32v20x_it.d \
./User/system_ch32v20x.d 

OBJS += \
./User/ch32v20x_it.o \
./User/system_ch32v20x.o 

DIR_OBJS += \
./User/*.o \

DIR_DEPS += \
./User/*.d \

DIR_EXPANDS += \
./User/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Debug" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Core" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/User" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Peripheral/inc" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/include" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/Common" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

