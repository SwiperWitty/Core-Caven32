################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/ADC.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Base.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Exist_GPIO.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Flash.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/IIC.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/PWM.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/SPI.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Sys_Time.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/uart.c 

OBJS += \
./Base_CH32V203/ADC.o \
./Base_CH32V203/Base.o \
./Base_CH32V203/Exist_GPIO.o \
./Base_CH32V203/Flash.o \
./Base_CH32V203/IIC.o \
./Base_CH32V203/PWM.o \
./Base_CH32V203/SPI.o \
./Base_CH32V203/Sys_Time.o \
./Base_CH32V203/uart.o 

C_DEPS += \
./Base_CH32V203/ADC.d \
./Base_CH32V203/Base.d \
./Base_CH32V203/Exist_GPIO.d \
./Base_CH32V203/Flash.d \
./Base_CH32V203/IIC.d \
./Base_CH32V203/PWM.d \
./Base_CH32V203/SPI.d \
./Base_CH32V203/Sys_Time.d \
./Base_CH32V203/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Base_CH32V203/ADC.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/ADC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/Base.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Base.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/Exist_GPIO.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Exist_GPIO.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/Flash.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Flash.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/IIC.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/IIC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/PWM.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/PWM.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/SPI.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/SPI.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/Sys_Time.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/Sys_Time.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/uart.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/uart.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

