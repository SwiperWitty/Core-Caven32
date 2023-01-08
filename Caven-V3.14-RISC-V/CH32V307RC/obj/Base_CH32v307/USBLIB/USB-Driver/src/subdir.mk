################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_core.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_init.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_int.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_mem.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_regs.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_sil.c 

OBJS += \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_core.o \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_init.o \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_int.o \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_mem.o \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_regs.o \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_sil.o 

C_DEPS += \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_core.d \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_init.d \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_int.d \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_mem.d \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_regs.d \
./Base_CH32v307/USBLIB/USB-Driver/src/usb_sil.d 


# Each subdirectory must supply rules for building sources it contributes
Base_CH32v307/USBLIB/USB-Driver/src/usb_core.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_core.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/USB-Driver/src/usb_init.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_init.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/USB-Driver/src/usb_int.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_int.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/USB-Driver/src/usb_mem.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_mem.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/USB-Driver/src/usb_regs.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_regs.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/USB-Driver/src/usb_sil.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/USB-Driver/src/usb_sil.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

