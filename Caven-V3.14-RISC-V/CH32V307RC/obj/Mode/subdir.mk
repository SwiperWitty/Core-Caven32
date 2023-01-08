################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/BZZ.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Control_Mode.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/HC595.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/LED.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_MP3.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Motor.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Time.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Tracing.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_UART.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Ultrasonic.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Mode.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Steering_engine.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/key.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/lcd.c \
E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/mlx90614.c 

OBJS += \
./Mode/BZZ.o \
./Mode/Control_Mode.o \
./Mode/HC595.o \
./Mode/LED.o \
./Mode/MODE_MP3.o \
./Mode/MODE_Motor.o \
./Mode/MODE_Time.o \
./Mode/MODE_Tracing.o \
./Mode/MODE_UART.o \
./Mode/MODE_Ultrasonic.o \
./Mode/Mode.o \
./Mode/Steering_engine.o \
./Mode/key.o \
./Mode/lcd.o \
./Mode/mlx90614.o 

C_DEPS += \
./Mode/BZZ.d \
./Mode/Control_Mode.d \
./Mode/HC595.d \
./Mode/LED.d \
./Mode/MODE_MP3.d \
./Mode/MODE_Motor.d \
./Mode/MODE_Time.d \
./Mode/MODE_Tracing.d \
./Mode/MODE_UART.d \
./Mode/MODE_Ultrasonic.d \
./Mode/Mode.d \
./Mode/Steering_engine.d \
./Mode/key.d \
./Mode/lcd.d \
./Mode/mlx90614.d 


# Each subdirectory must supply rules for building sources it contributes
Mode/BZZ.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/BZZ.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/Control_Mode.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Control_Mode.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/HC595.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/HC595.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/LED.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/LED.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_MP3.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_MP3.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_Motor.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Motor.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_Time.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Time.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_Tracing.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Tracing.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_UART.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_UART.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/MODE_Ultrasonic.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/MODE_Ultrasonic.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/Mode.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Mode.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/Steering_engine.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/Steering_engine.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/key.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/key.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/lcd.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/lcd.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
Mode/mlx90614.o: E:/星辰大海/Software/Core-Caven32/MCU_LIB-Cloud-/Mode/mlx90614.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Debug" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Core" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\User" -I"E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Peripheral\inc" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Mode" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\API" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\CONFIG" -I"E:\星辰大海\Software\Core-Caven32\MCU_LIB-Cloud-\Base_CH32v307\USBLIB\USB-Driver\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

