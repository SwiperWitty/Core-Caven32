################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

