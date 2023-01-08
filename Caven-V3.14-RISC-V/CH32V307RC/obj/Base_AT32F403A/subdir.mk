################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/ADC.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Base.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/DAC.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Encoder.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/IIC.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/PWM.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/SPI.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Sys_Time.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/stmflash.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/uart.c 

OBJS += \
./Base_AT32F403A/ADC.o \
./Base_AT32F403A/Base.o \
./Base_AT32F403A/DAC.o \
./Base_AT32F403A/Encoder.o \
./Base_AT32F403A/IIC.o \
./Base_AT32F403A/PWM.o \
./Base_AT32F403A/SPI.o \
./Base_AT32F403A/Sys_Time.o \
./Base_AT32F403A/stmflash.o \
./Base_AT32F403A/uart.o 

C_DEPS += \
./Base_AT32F403A/ADC.d \
./Base_AT32F403A/Base.d \
./Base_AT32F403A/DAC.d \
./Base_AT32F403A/Encoder.d \
./Base_AT32F403A/IIC.d \
./Base_AT32F403A/PWM.d \
./Base_AT32F403A/SPI.d \
./Base_AT32F403A/Sys_Time.d \
./Base_AT32F403A/stmflash.d \
./Base_AT32F403A/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Base_AT32F403A/ADC.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/ADC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/Base.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Base.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/DAC.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/DAC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/Encoder.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Encoder.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/IIC.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/IIC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/PWM.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/PWM.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/SPI.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/SPI.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/Sys_Time.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/Sys_Time.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/stmflash.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/stmflash.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_AT32F403A/uart.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_AT32F403A/uart.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_AT32F403A" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

