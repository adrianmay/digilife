SECTIONS
{
	.text :
	{
		more.o (.text)
		main.o (.text)
		handlers.o (.text)
		peripherals.o (.text)
	}
}