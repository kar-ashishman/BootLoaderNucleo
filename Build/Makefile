CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CPU = cortex-m3
STD = gnu11
BOARD = stm32vldiscovery
CFLAGS = -mthumb -mcpu=$(CPU) -g -c
LDFLAGS = -T rom.ld -Map=final.map

SRC = startup.c main.c
OBJ = $(patsubst %.c,%.o,$(SRC))

final.elf : $(OBJ) rom.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
	arm-none-eabi-objdump -D -S final.elf > final.elf.lst
	arm-none-eabi-readelf -a final.elf > final.elf.debug

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@


qemu :
	qemu-system-arm -S -M $(BOARD) -nographic -cpu $(CPU) -kernel final.elf -gdb tcp::1234

gdb :
	gdb-multiarch -q final.elf -ex "target remote localhost:1234"

.PHONY : clean

clean :
	rm -f *.o *.elf *.map *.lst *.debug

