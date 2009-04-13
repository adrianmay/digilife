#include "zed.h"

void main()
{
	int a;
	//put_handler(32, isr_nothing, GATE_DEFAULT);
	clrscr();
	printfoo();
//	crash();
//	a = 1/0;
	*((int*)0xffffffff)=1;
	printbar();
	for(;;);
}

const char *tutorial3 = "MuOS Tutorial 3";
