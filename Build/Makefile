CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
ARFLAG = rcs
CPU = cortex-m4
STD = gnu11
OPT = 0
CFLAGS = -mcpu=$(CPU) -std=$(STD) -mthumb -std=gnu11 -Wall -Wextra -O$(OPT)
# -Wl is given as we are using gcc to link. if we use LD then -Wl is not needed 
LDFLAGS = -nostdlib -T rom.ld -Wl,-Map=final.map

SOURCE := $(wildcard *.c)
OBJ := $(patsubst %.c,objects/%.o,$(SOURCE))
LIB := objects/lib.a

all : final.elf
	make hexdump

final.elf : $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^

$(LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

objects/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<



.PHONY : clean objdumpstartup objdumpelf hexdump
objdumpstartup : startup.o
	arm-none-eabi-objdump -h startup.o
objdumpelf : final.elf
	arm-none-eabi-objdump -h final.elf
hexdump : final.elf
	arm-none-eabi-objcopy -O ihex final.elf final.hex
clean:
	del /f *.elf *.map *.hex
	del objects\*.o
	del objects\*.a