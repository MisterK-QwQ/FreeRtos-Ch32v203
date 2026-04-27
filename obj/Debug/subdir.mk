################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

C_DEPS += \
./Debug/debug.d 

OBJS += \
./Debug/debug.o 

DIR_OBJS += \
./Debug/*.o \

DIR_DEPS += \
./Debug/*.d \

DIR_EXPANDS += \
./Debug/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Debug" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Core" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/User" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Peripheral/inc" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/include" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/Common" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

