extern void crash();
char tank_stack[1000];
char tank_stack0[1000];
unsigned char * p = 0xB8000-0xc000; 

void tank_idle()
{
	*p='T';
	while(1) {}
}

void flash()
{
	p[1] = ~p[1];
	static int count=10;
	if (!--count);
	//crash();
}

void tank_main()
{
	int i;
	while(1)
	{
		flash();
		for (i=0;i<1500000;i++); //blimey that's fast
	}
}

