#include "zed.h"
/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened!  All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */

int ticks=0;
void interrupt_handler(struct registers *r)
{
    /* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
        print(faultmsg[r->int_no]);
        print(" Exception. System Halted!\n");
        for (;;);
    }
    else if (r->int_no==32)
    {
	    ticks = (ticks+1)%100;
	    if (!ticks)
		print("tock ");
    }
    else if (r->int_no==33)
	    keyboard_handler();
}

const char faultmsg[32][20] = 
{
	"Divide",
	"Debug",
	"NMI",
	"Breakpoint",
	"Overflow",
	"Bounds",
	"Unknown opcode",
	"No math copro",
	"Double",
	"FP segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack",
	"General protection",
	"Page",
	"Reserved",
	"Math",
	"Alignment",
	"Machine check",
	"SIMD",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};




