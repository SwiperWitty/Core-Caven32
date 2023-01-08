################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/hw_config.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_desc.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_endp.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_istr.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_prop.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_pwr.c 

OBJS += \
./Base_CH32v307/USBLIB/CONFIG/hw_config.o \
./Base_CH32v307/USBLIB/CONFIG/usb_desc.o \
./Base_CH32v307/USBLIB/CONFIG/usb_endp.o \
./Base_CH32v307/USBLIB/CONFIG/usb_istr.o \
./Base_CH32v307/USBLIB/CONFIG/usb_prop.o \
./Base_CH32v307/USBLIB/CONFIG/usb_pwr.o 

C_DEPS += \
./Base_CH32v307/USBLIB/CONFIG/hw_config.d \
./Base_CH32v307/USBLIB/CONFIG/usb_desc.d \
./Base_CH32v307/USBLIB/CONFIG/usb_endp.d \
./Base_CH32v307/USBLIB/CONFIG/usb_istr.d \
./Base_CH32v307/USBLIB/CONFIG/usb_prop.d \
./Base_CH32v307/USBLIB/CONFIG/usb_pwr.d 


# Each subdirectory must supply rules for building sources it contributes
Base_CH32v307/USBLIB/CONFIG/hw_config.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/hw_config.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/CONFIG/usb_desc.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_desc.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/CONFIG/usb_endp.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_endp.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/CONFIG/usb_istr.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_istr.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/CONFIG/usb_prop.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_prop.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Base_CH32v307/USBLIB/CONFIG/usb_pwr.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Base_CH32v307/USBLIB/CONFIG/usb_pwr.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

