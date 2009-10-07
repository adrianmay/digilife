ENTRY(start)
SECTIONS
{
	.text 0x8000 :
	{
		more.o (.text)
		zed.o (.text)
		rand.o (.text)
	}
	KERNEL_CODE_END = .;
	.data :
	{
		more.o (.data)
		zed.o (.data)
		rand.o (.data)
	}
	.bss :  
	{ 
		more.o (.bss)  
		zed.o (.bss)  
		rand.o (.bss)

	}
	KERNEL_END = .;
	. = ALIGN(512);
	TANK_START = .;
	.tank 0 :
	AT(0xd000)
	{
		tanks.o (.text)
		tanks.o (.data)
	}
	TANK_END = .;
	TANK_SIZE = SIZEOF(.tank);
	
}
