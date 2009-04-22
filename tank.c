char tank_stack[1000];
unsigned char * p = 0xB8001-0xc000; 
void flash()
{
	*p = ~*p;
}

void tank_main()
{
	int i;
	while(1)
	{
		for (i=0;i<100000;i++);
		flash();
		
	}
}
