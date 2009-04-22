char tank_stack[1000];
char tank_stack0[1000];
unsigned char * p = 0xB8000-0xc000; 
void flash()
{
	p[1] = ~p[1];
}

void tank_main()
{
	*p='G';
	int i;
	while(1)
	{
		for (i=0;i<150000000;i++); //blimey that's fast
		flash();
	}
}
