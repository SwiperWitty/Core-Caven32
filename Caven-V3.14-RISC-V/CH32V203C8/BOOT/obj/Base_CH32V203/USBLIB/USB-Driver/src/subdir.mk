################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_core.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_init.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_int.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_mem.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_regs.c \
E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_sil.c 

OBJS += \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_core.o \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_init.o \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_int.o \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_mem.o \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_regs.o \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_sil.o 

C_DEPS += \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_core.d \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_init.d \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_int.d \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_mem.d \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_regs.d \
./Base_CH32V203/USBLIB/USB-Driver/src/usb_sil.d 


# Each subdirectory must supply rules for building sources it contributes
Base_CH32V203/USBLIB/USB-Driver/src/usb_core.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_core.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/USBLIB/USB-Driver/src/usb_init.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_init.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/USBLIB/USB-Driver/src/usb_int.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_int.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/USBLIB/USB-Driver/src/usb_mem.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_mem.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/USBLIB/USB-Driver/src/usb_regs.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_regs.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32V203/USBLIB/USB-Driver/src/usb_sil.o: E:/WORK/Core-Caven32/MCU_LIB-Cloud-/Base_CH32V203/USBLIB/USB-Driver/src/usb_sil.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Debug" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Core" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\User" -I"E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Peripheral\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\CONFIG" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203\USBLIB\USB-Driver\inc" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Magic_API" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Base_CH32V203" -I"E:\WORK\Core-Caven32\MCU_LIB-Cloud-\Mode" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

