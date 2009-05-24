TARGET = zed
CC = gcc
CCL = ld
ASM = nasm

CFLAGS = -ffreestanding 
# LINKFLAGS = -nostartfiles -Ttext 0x8000 -Map $(TARGET).map
LINKFLAGS = -nostartfiles -T zed.lds -Map $(TARGET).map
ASMFLAGS = -f bin
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CCL) $(LINKFLAGS) 
ASSEMBLE = $(ASM) $(ASMFLAGS)

all: $(TARGET).img restore

makeboot.exe: makeboot.C Makefile
	gcc -o makeboot.exe -x c makeboot.C -x none

OBJFILES := more.o zed.o tank.o tanks.o

$(TARGET).img: makeboot.exe bootsect.bin kernel.bin
	./makeboot.exe $(TARGET).img bootsect.bin kernel.bin

kernel.o: bootsect.bin $(OBJFILES) zed.lds
	$(LINK) -o kernel.o $(OBJFILES) 
	

kernel.bin: kernel.o
	objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

%.o: %.c Makefile
	$(COMPILE) -o $@ $<

%.o: %.s Makefile
	$(ASM) -f elf -o $@ $<

bootsect.bin: bootsect.asm
	$(ASSEMBLE) -o $@ $<

restore: 
	rm nvram *.log *.vmsd *.vmem *.vmss *.vmxf 2> /dev/null;  true
clean: restore
	rm *.exe *.o *.img *.bin *.map *~ 2> /dev/null;  true
 
run: 
	/usr/bin/vmplayer zed.vmx
	
	
