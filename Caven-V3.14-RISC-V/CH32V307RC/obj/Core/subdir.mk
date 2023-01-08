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
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

