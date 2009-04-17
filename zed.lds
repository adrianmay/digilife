ENTRY(start)
SECTIONS
{
	.text 0x8000 :
	{
		more.o (.text)
		main.o (.text)
		handlers.o (.text)
		peripherals.o (.text)
	}
	.data :
	{
		more.o (.data)
		*.o (.data)
	}
	.bss :  { *(.bss)  *(COMMON) }
}