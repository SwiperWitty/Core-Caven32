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
-include Magic_API/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include Base_CH32V203/USBLIB/USB-Driver/src/subdir.mk
-include Base_CH32V203/USBLIB/CONFIG/subdir.mk
-include Base_CH32V203/subdir.mk
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
CH32V203C8T6.hex \

SECONDARY_LIST += \
CH32V203C8T6.lst \

SECONDARY_SIZE += \
CH32V203C8T6.siz \


# 所有目标
all: CH32V203C8T6.elf secondary-outputs

# 工具调用
CH32V203C8T6.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -T "E:\WORK\Core-Caven32\Caven-V3.14-RISC-V\CH32V203C8\BOOT\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"CH32V203C8T6.map" --specs=nano.specs --specs=nosys.specs -o "CH32V203C8T6.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
	@	@
CH32V203C8T6.hex: CH32V203C8T6.elf
	@	riscv-none-embed-objcopy -O ihex "CH32V203C8T6.elf"  "CH32V203C8T6.hex"
	@	@
CH32V203C8T6.lst: CH32V203C8T6.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble "CH32V203C8T6.elf" > "CH32V203C8T6.lst"
	@	@
CH32V203C8T6.siz: CH32V203C8T6.elf
	@	riscv-none-embed-size --format=berkeley "CH32V203C8T6.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) CH32V203C8T6.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
