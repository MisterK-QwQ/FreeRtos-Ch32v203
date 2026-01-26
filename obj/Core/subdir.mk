################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

C_DEPS += \
./Core/core_riscv.d 

OBJS += \
./Core/core_riscv.o 

DIR_OBJS += \
./Core/*.o \

DIR_DEPS += \
./Core/*.d \

DIR_EXPANDS += \
./Core/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/Debug" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/Core" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/User" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/Peripheral/inc" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/include" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/portable" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/portable/Common" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/portable/GCC/RISC-V" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions" -I"c:/Users/admin/mounriver-studio-projects/CH32V203Template/FreeRTOS/portable/MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

