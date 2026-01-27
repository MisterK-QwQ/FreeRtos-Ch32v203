################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../User/Utils/TimerUtil.cpp 

CPP_DEPS += \
./User/Utils/TimerUtil.d 

OBJS += \
./User/Utils/TimerUtil.o 

DIR_OBJS += \
./User/Utils/*.o \

DIR_DEPS += \
./User/Utils/*.d \

DIR_EXPANDS += \
./User/Utils/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/Utils/%.o: ../User/Utils/%.cpp
	@	riscv-none-embed-g++ -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -I"d:/文档/MounRiver/CH32V203Template/Core" -I"d:/文档/MounRiver/CH32V203Template/Debug" -I"d:/文档/MounRiver/CH32V203Template/Peripheral/inc" -I"d:/文档/MounRiver/CH32V203Template/User" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/include" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/Common" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/GCC" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/MemMang" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/GCC/RISC-V" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions" -I"d:/文档/MounRiver/CH32V203Template/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -std=c++17 -fabi-version=0 -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

