################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Mode/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include Base_CH32v307/USBLIB/USB-Driver/src/subdir.mk
-include Base_CH32v307/USBLIB/CONFIG/subdir.mk
-include Base_CH32v307/subdir.mk
-include API/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Caven.hex \

SECONDARY_LIST += \
Caven.lst \

SECONDARY_SIZE += \
Caven.siz \


# 所有目标
all: Caven.elf secondary-outputs

# 工具调用
Caven.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -T "E:\星辰大海\Software\Core-Caven32\Caven-V3.14-RISC-V\CH32V307RC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"Caven.map" --specs=nano.specs --specs=nosys.specs  -o "Caven.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf  -lprintfloat
	@	@
Caven.hex: Caven.elf
	@	riscv-none-embed-objcopy -O ihex "Caven.elf"  "Caven.hex"
	@	@
Caven.lst: Caven.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble "Caven.elf" > "Caven.lst"
	@	@
Caven.siz: Caven.elf
	@	riscv-none-embed-size --format=berkeley "Caven.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Caven.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
