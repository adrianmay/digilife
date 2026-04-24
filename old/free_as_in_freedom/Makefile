TARGET = zed
CC = gcc
CCL = ld
ASM = nasm
# BOCHS=/home/ad/junk/build/b/bochs
BOCHS=bochs

CFLAGS = -ffreestanding -m32 -fno-stack-protector
# LINKFLAGS = -nostartfiles -Ttext 0x8000 -Map $(TARGET).map
LINKFLAGS = -melf_i386 -nostartfiles -T zed.lds -Map $(TARGET).map
ASMFLAGS = -f bin
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CCL) $(LINKFLAGS) 
ASSEMBLE = $(ASM) $(ASMFLAGS)

run: zed.img bochsrc 30M.sample
	$(BOCHS) -qf bochsrc

all: $(TARGET).img restore

makeboot.exe: makeboot.C Makefile
	gcc -o makeboot.exe -x c makeboot.C -x none

OBJFILES := more.o zed.o tanks.o rand.o

$(TARGET).img: makeboot.exe bootsect.bin kernel.bin
	./makeboot.exe $(TARGET).img bootsect.bin kernel.bin

kernel.o: bootsect.bin $(OBJFILES) zed.lds
	$(LINK) -o kernel.o $(OBJFILES) 
	

kernel.bin: kernel.o
	objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

%.o: %.c Makefile header.h
	$(COMPILE) -o $@ $<

%.o: %.s Makefile
	$(ASM) -f elf -o $@ $<

bootsect.bin: bootsect.asm
	$(ASSEMBLE) -o $@ $<

30M.sample:
	dd if=/dev/zero of=$@ bs=$$((1024*1024)) count=30

restore: 
	rm -rf nvram *.log *.vmsd *.vmem *.vmss *.vmxf 2> /dev/null;  true
clean: restore
	rm -rf *.exe *.o *.img *.bin *.map *~ 2> /dev/null;  true
 
	
	
