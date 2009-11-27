ENTRY(start)
SECTIONS
{
	.text 0x8000 :
	{
		more.o (.text)
		rand.o (.text)
		zed.o (.text)
	}
	. = ALIGN(16);
	KERNEL_CODE_END = .;
	KERNEL_DATA_BEGIN = .;
	.data :
	{
		more.o (.data)
		rand.o (.data)
		zed.o (.data)
	}
	.bss :  
	{ 
		more.o (.bss)  
		rand.o (.bss)
		zed.o (.bss)
	}
	. = ALIGN(512);
	KERNEL_END = .;
	TANK_START = .;
	.tank 0 :
	AT(0x10000)
	{
		tanks.o (.text)
		tanks.o (.data)
	}
	TANK_END = .;
	TANK_SIZE = SIZEOF(.tank);
	KERNEL_DATA_SIZE = KERNEL_END - KERNEL_DATA_BEGIN;
	
}
