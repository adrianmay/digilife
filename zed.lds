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
}