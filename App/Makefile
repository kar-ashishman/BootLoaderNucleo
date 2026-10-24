# Compiler and tools
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
AR := arm-none-eabi-ar rcs

# Project modules
MODULES := Src app hal
.DEFAULT_GOAL := all

# Common directories
PUB_DIR := public
BIN_DIR := bin
DEBUG_DIR := debug

# Flags
INCLUDES := $(foreach module,$(MODULES),-I$(module)/$(PUB_DIR))
CFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -c -Wall -g -Wl,--gc-sections --specs=nosys.specs

# Object and archive rules per module
define MODULE_template
$1_LIB  := $1/$(BIN_DIR)/$1.a

$$($1_LIB): $$($1_OBJS)
	@if not exist "$$(@D)" mkdir "$$(@D)"
	$(AR) $$@ $$^
endef

# Source and object paths differ between the modules in this project.
hal_SRCS := $(wildcard hal/src/*.c)
hal_OBJS := $(patsubst hal/src/%.c,hal/bin/%.o,$(hal_SRCS))
Src_SRCS := $(wildcard Src/*.c)
Src_OBJS := $(patsubst Src/%.c,Src/bin/%.o,$(Src_SRCS))
app_SRCS := $(wildcard app/source/*.c)
app_OBJS := $(patsubst app/source/%.c,app/bin/%.o,$(app_SRCS))

$(foreach module,$(MODULES),$(eval $(call MODULE_template,$(module))))

hal/bin/%.o: hal/src/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

Src/bin/%.o: Src/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

app/bin/%.o: app/source/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Final executable
TARGET := $(DEBUG_DIR)/debug.elf
ALL_LIBS := $(foreach module,$(MODULES),$(module)/$(BIN_DIR)/$(module).a)
LINKER_FLAGS := -T STM32F446RETX_FLASH.ld --specs=nosys.specs -Wl,-Map=$(TARGET).map
FLASHHEX := $(DEBUG_DIR)/debug.hex

$(TARGET): $(ALL_LIBS)
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(LINKER_FLAGS) -o $@ $^

$(FLASHHEX): $(TARGET)
	$(OBJCOPY) -O ihex $< $@


# Phony targets
.PHONY: all clean $(MODULES)

all: $(FLASHHEX)

app: app/$(BIN_DIR)/app.a
HAL: HAL/$(BIN_DIR)/HAL.a

clean:
	@if exist "$(DEBUG_DIR)" rmdir /s /q "$(DEBUG_DIR)"
	@for %%M in ($(MODULES)) do @if exist "%%M\$(BIN_DIR)" rmdir /s /q "%%M\$(BIN_DIR)"
	@for %%M in ($(MODULES)) do @if exist "%%M\$(BIN_DIR)\%%M.a" del /q "%%M\$(BIN_DIR)\%%M.a"
