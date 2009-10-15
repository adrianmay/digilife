ENTRY(start)
SECTIONS
{
	.text 0x8000 :
	{
		more.o (.text)
		rand.o (.text)
		zed.o (.text)
	}
	KERNEL_CODE_END = .;
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
	KERNEL_END = .;
	. = ALIGN(512);
	TANK_START = .;
	.tank 0 :
	AT(0x20000)
	{
		tanks.o (.text)
		tanks.o (.data)
	}
	TANK_END = .;
	TANK_SIZE = SIZEOF(.tank);
	
}
