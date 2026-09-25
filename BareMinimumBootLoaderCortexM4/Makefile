# Compiler and tools
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
AR := arm-none-eabi-ar rcs

# Project modules
MODULES := Boot Build HAL
.DEFAULT_GOAL := all

# Common directories
SRC_DIR := src
PUB_DIR := public
BIN_DIR := bin
DEBUG_DIR := debug

# Flags
INCLUDES := $(foreach module,$(MODULES),-I$(module)/$(PUB_DIR))
CFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -c -Wall -g

# Object and archive rules per module
define MODULE_template
$1_SRCS := $(wildcard $1/$(SRC_DIR)/*.c)
$1_OBJS := $$(patsubst $1/$(SRC_DIR)/%.c,$1/$(BIN_DIR)/%.o,$$($1_SRCS))
$1_LIB  := $1/$(BIN_DIR)/$1.a

$$($1_LIB): $$($1_OBJS)
	$(AR) $$@ $$^

$1/$(BIN_DIR)/%.o: $1/$(SRC_DIR)/%.c
	@if not exist "$$(@D)" mkdir "$$(@D)"
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

$(foreach module,$(MODULES),$(eval $(call MODULE_template,$(module))))

# Final executable
TARGET := $(DEBUG_DIR)/debug.elf
ALL_LIBS := $(foreach module,$(MODULES),$(module)/$(BIN_DIR)/$(module).a)
LINKER_FLAGS := -T Build/STM32F466RE_Flash.ld --specs=nosys.specs -Wl,-Map=$(TARGET).map
FLASHHEX := $(DEBUG_DIR)/debug.hex

$(TARGET): $(ALL_LIBS)
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(LINKER_FLAGS) -o $@ $^

$(FLASHHEX): $(TARGET)
	$(OBJCOPY) -O ihex $< $@


# Phony targets
.PHONY: all clean $(MODULES)

all: $(FLASHHEX)

Boot: Boot/$(BIN_DIR)/Boot.a
Build: Build/$(BIN_DIR)/Build.a
HAL: HAL/$(BIN_DIR)/HAL.a

clean:
	@if exist "$(DEBUG_DIR)" rmdir /s /q "$(DEBUG_DIR)"
	@for %%M in ($(MODULES)) do @if exist "%%M\$(BIN_DIR)" rmdir /s /q "%%M\$(BIN_DIR)"
	@for %%M in ($(MODULES)) do @if exist "%%M\$(BIN_DIR)\%%M.a" del /q "%%M\$(BIN_DIR)\%%M.a"
