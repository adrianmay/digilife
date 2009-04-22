char tank_stack[0x1000];
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
		for (i=0;i<150000000;i++); //blimey that's fast
		flash();
		
	}
}
