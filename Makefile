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

all: clean $(TARGET).img

makeboot: makeboot.C
	gcc -o makeboot -x c makeboot.C -x none

OBJFILES := $(patsubst %.c,%.o,$(wildcard *.c)) 


$(TARGET).img: makeboot bootsect.bin kernel.bin
	./makeboot $(TARGET).img bootsect.bin kernel.bin

kernel.o: bootsect.bin $(OBJFILES)
	$(LINK) -o kernel.o $(OBJFILES) 
	

kernel.bin: kernel.o
	objcopy -R .note -R .comment -S -O binary kernel.o kernel.bin

%.o: %.c
	$(COMPILE) -o $@ $<

bootsect.bin: bootsect.asm
	$(ASSEMBLE) -o $@ $<

clean:
	rm makeboot *.o *.img *.bin *.map 2> /dev/null;  true
 
