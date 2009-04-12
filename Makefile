TARGET = zed
CC = gcc
CCL = ld
ASM = nasm

CFLAGS = -ffreestanding 
LINKFLAGS = -nostartfiles -e start -Ttext 0x1000 -Map $(TARGET).map
ASMFLAGS = -f bin
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CCL) $(LINKFLAGS) 
ASSEMBLE = $(ASM) $(ASMFLAGS)

all: $(TARGET).img

makeboot.exe: makeboot.C
	gcc -o makeboot.exe -x c makeboot.C -x none

OBJFILES := more.o main.o handlers.o video.o ports.o

$(TARGET).img: makeboot.exe bootsect.bin kernel.bin
	./makeboot.exe $(TARGET).img bootsect.bin kernel.bin

kernel.o: bootsect.bin $(OBJFILES)
	$(LINK) -o kernel.o $(OBJFILES) 
	

kernel.bin: kernel.o
	objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

%.o: %.c
	$(COMPILE) -o $@ $<

%.o: %.s
	$(ASM) -f elf -o $@ $<

bootsect.bin: bootsect.asm
	$(ASSEMBLE) -o $@ $<

clean:
	rm *.exe *.o *.img *.bin *.map 2> /dev/null;  true
 
