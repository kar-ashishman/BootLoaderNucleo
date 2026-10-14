# ==============================================================================
# OS Detection Block (Cross-Platform compatibility for Windows and Linux)
# ==============================================================================
ifeq ($(OS),Windows_NT)
    # Windows Settings
    MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RMDIR = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
    RMFILE = if exist "$(subst /,\,$(1))" del /q "$(subst /,\,$(1))"
    # Helper to execute clean loop under Windows cmd shell
    CLEAN_MODULES = @for %%M in ($(MODULES)) do ( \
        if exist "%%M\$(BIN_DIR)" rmdir /s /q "%%M\$(BIN_DIR)" \
    )
else
    # Linux / macOS / GitHub Actions Settings
    MKDIR = mkdir -p $(1)
    RMDIR = rm -rf $(1)
    RMFILE = rm -f $(1)
    # Helper to execute clean loop under Linux/Bash shell
    CLEAN_MODULES = @for module in $(MODULES); do \
        rm -rf $$module/$(BIN_DIR); \
    done
endif

# ==============================================================================
# Toolchain and project variables
# ==============================================================================
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
AR := arm-none-eabi-ar rcs

MODULES := Boot Build HAL
.DEFAULT_GOAL := all

SRC_DIR := src
PUB_DIR := public
BIN_DIR := bin
DEBUG_DIR := debug

INCLUDES := $(foreach module,$(MODULES),-I$(module)/$(PUB_DIR))
CFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -c -Wall -g -Wl,--gc-sections --specs=nosys.specs

# ==============================================================================
# Rules Definition (Using dynamic function calls for cross-platform variables)
# ==============================================================================
define MODULE_template
$1_SRCS := $(wildcard $1/$(SRC_DIR)/*.c)
$1_OBJS := $$(patsubst $1/$(SRC_DIR)/%.c,$1/$(BIN_DIR)/%.o,$$($1_SRCS))
$1_LIB  := $1/$(BIN_DIR)/$1.a

$$($1_LIB): $$($1_OBJS)
	$(AR) $$@ $$^

$1/$(BIN_DIR)/%.o: $1/$(SRC_DIR)/%.c
	@$(call MKDIR,$$(@D))
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

$(foreach module,$(MODULES),$(eval $(call MODULE_template,$(module))))

# Final targets
TARGET := $(DEBUG_DIR)/debug.elf
ALL_LIBS := $(foreach module,$(MODULES),$(module)/$(BIN_DIR)/$(module).a)
LINKER_FLAGS := -T Build/STM32F466RE_Flash.ld --specs=nosys.specs -Wl,-Map=$(TARGET).map
FLASHHEX := $(DEBUG_DIR)/debug.hex

$(TARGET): $(ALL_LIBS)
	@$(call MKDIR,$(@D))
	$(CC) $(LINKER_FLAGS) -o $@ $^

$(FLASHHEX): $(TARGET)
	$(OBJCOPY) -O ihex $< $@

# ==============================================================================
# Phony Targets
# ==============================================================================
.PHONY: all clean $(MODULES)

all: $(FLASHHEX)

Boot: Boot/$(BIN_DIR)/Boot.a
Build: Build/$(BIN_DIR)/Build.a
HAL: HAL/$(BIN_DIR)/HAL.a

clean:
	@$(call RMDIR,$(DEBUG_DIR))
	$(CLEAN_MODULES)
