################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../User/Io/GpioManager.cpp 

CPP_DEPS += \
./User/Io/GpioManager.d 

OBJS += \
./User/Io/GpioManager.o 

DIR_OBJS += \
./User/Io/*.o \

DIR_DEPS += \
./User/Io/*.d \

DIR_EXPANDS += \
./User/Io/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/Io/%.o: ../User/Io/%.cpp
	@	riscv-none-embed-g++ -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Core" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Debug" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/Peripheral/inc" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/User" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/include" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/Common" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/MemMang" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions" -I"d:/文档/MounRiver/FreeRtos-Ch32v203/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

