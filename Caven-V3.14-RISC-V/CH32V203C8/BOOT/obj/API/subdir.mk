################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/API.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Check_CRC16.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/DATA_Handle.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Keyboard.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Time_Handle.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/libcollections.c 

OBJS += \
./API/API.o \
./API/Check_CRC16.o \
./API/DATA_Handle.o \
./API/Keyboard.o \
./API/Time_Handle.o \
./API/libcollections.o 

C_DEPS += \
./API/API.d \
./API/Check_CRC16.d \
./API/DATA_Handle.d \
./API/Keyboard.d \
./API/Time_Handle.d \
./API/libcollections.d 


# Each subdirectory must supply rules for building sources it contributes
API/API.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/API.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
API/Check_CRC16.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Check_CRC16.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
API/DATA_Handle.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/DATA_Handle.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
API/Keyboard.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Keyboard.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
API/Time_Handle.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/Time_Handle.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
API/libcollections.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/API/libcollections.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

