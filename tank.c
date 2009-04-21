unsigned char * p = 0xB8001; 
char blah[]="WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW";
void flash()
{
	*p = ~*p;
}

void tank_main()
{
	int i;
	while(1)
	{
		for (i=0;i<100000000;i++);
		flash();
		
	}
}
