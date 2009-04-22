ENTRY(start)
SECTIONS
{
	.text 0x8000 :
	{
		more.o (.text)
		zed.o (.text)
}
	KERNEL_CODE_END = .;
	.data :
	{
		more.o (.data)
		zed.o (.data)
	}
	.bss :  { *(.bss)  *(COMMON) }
	KERNEL_END = .;
	TANK_START = .;
	.tank 0 :
	AT(0xc000)
	{
		tank.o (.text)
		tank.o (.data)		
	}
	TANK_END = .;
	TANK_SIZE = SIZEOF(.tank);
	
}