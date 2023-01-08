################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/main.c \
../User/system_ch32v20x.c 

OBJS += \
./User/ch32v20x_it.o \
./User/main.o \
./User/system_ch32v20x.o 

C_DEPS += \
./User/ch32v20x_it.d \
./User/main.d \
./User/system_ch32v20x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

