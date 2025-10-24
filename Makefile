# === Makefile (part A) ===
# Toolchain (bare‑metal ARM)
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE    := arm-none-eabi-size

# Target CPU: Cortex‑M0 (STM32F072)
MCUFLAGS := -mcpu=cortex-m0 -mthumb -mfloat-abi=soft

# Common compile options (no stdlib; freestanding)
CFLAGS  := $(MCUFLAGS) -O0 -g3 -ffreestanding -fno-builtin -Wall -Wextra -Werror
ASFLAGS := $(MCUFLAGS) -g3
LDFLAGS := $(MCUFLAGS) -Wl,-Map=build.map -nostartfiles -Wl,--gc-sections -T linker.ld

# Include path (we will add headers later)
INCLUDES := -Iinc

# For now, do NOT add build rules. We will append them in Step 4.
# Try: `make` now — it should fail with "No rule to make target" (expected).

# === Append (part B)===
SRC_C := $(wildcard src/*.c)
SRC_S := $(wildcard src/*.s)
OBJ   := $(SRC_C:.c=.o) $(SRC_S:.s=.o)
TARGET := main

.PHONY: all clean
all: $(TARGET).elf $(TARGET).bin

clean:
	rm -f src/*.o $(TARGET).elf $(TARGET).bin build.map

# === Append (part C) ===

# Compile rules
src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/%.o: src/%.s
	$(CC) $(ASFLAGS) -c $< -o $@

# Link rule
$(TARGET).elf: $(OBJ) linker.ld
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	$(SIZE) $@

# Binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@