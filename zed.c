#pragma pack (push, 1)
#include "header.h"

unsigned char spare_stack_block[STACKSIZE];

struct Task tasks[3];
const void * hack_from=&tasks[0].stack;

/* Access byte's flags */
#define ACS_PRESENT     0x80            /* present segment */
#define ACS_PRIV_0		0
#define ACS_PRIV_1		0x20
#define ACS_PRIV_2		0x40
#define ACS_PRIV_3		0x60
#define ACS_CSEG        0x18            /* code segment */
#define ACS_DSEG        0x10            /* data segment */
#define ACS_CONFORM     0x04            /* conforming segment */
#define ACS_READ        0x02            /* readable segment */
#define ACS_WRITE       0x02            /* writable segment */
#define ACS_GRAN		0x8000
#define ACS_32BIT		0x4000
#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ | ACS_32BIT) //409a
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE | ACS_32BIT) //4092
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE | ACS_32BIT) //4092
#define ACS_LIMH	0x100
#define ACS_LDT (ACS_PRESENT+2)
#define ACS_TASK_STATE (ACS_PRESENT+9)
#define ACS_TASK_GATE (ACS_PRESENT+5)
#define ACS_CALL_GATE (ACS_PRESENT+0x0c)
#define ACS_RUPT_GATE (ACS_PRESENT+0x0e)
#define ACS_TRAP_GATE (ACS_PRESENT+0x0f)


struct segment_descriptor gdt[GDT_MAX]=
{
	{0,0,0,0,0}, //null
	{0,0,0,ACS_CODE+ACS_LIMH,0}, //kernel code 8
	{0,0,0,ACS_DATA+ACS_LIMH*0x0c,0}, //kernel data 10
	{0,TANKAT,0,ACS_CODE+ACS_PRIV_3+ACS_LIMH*TANKPAGES,0}, //tank code 18
	{0,TANKAT,0,ACS_DATA+ACS_PRIV_3+ACS_LIMH*TANKPAGES,0}, //tank data 20
	{STACKSIZE,0,0,ACS_STACK+ACS_LIMH*0x0c,0}, //spare stack 28
	{0,0,0,0,0}, //kernel tss 30
	{STACKSIZE,0,0,ACS_STACK+ACS_LIMH*0x0c,0}, //kernel stack 38
	{0,0,0,0,0}, //tank tss 40
	{0,0,0,0,0}, //tank stack 48
	{0,0,0,0,0}, //keyboard tss 50
	{0,0,0,0,0}, //keyboard stack 58
	{0,0x40,0,0x85,0}, //task gate 60
	{80*25*2,0x8000,0xb, ACS_DATA+ACS_PRIV_3,0}, //screen 68
};


const void * hack_to=&gdt[kernel_stack].base_l;
const void * hack_too=&gdt[spare_stack].base_l;


struct gdt_descriptor gdt_desc={GDT_MAX*8-1, &gdt};


const char faultmsg[32][20];

void nuketank()
{
	int i;
	for (i=0;i<0x10000*TANKPAGES/4;i++)
		((unsigned int*)(TANKAT))[i]=rand();
}


void main()
{
	int a;
	setup_tasks();
	clrscr();
	randinit();
	nuketank();
	do_histogram();
loopmain:
	goto loopmain;
	//put_handler(32, isr_nothing, GATE_DEFAULT);
	jump_tank();
//main_loop:
	//printbar();
	//__asm("iret");
	//goto main_loop;
	
}

void setup_task(int which, int ring, int cs, int ds, void * ip, int ss0, int sp0, int rupt)
{
	struct TSS * task = &tasks[which].tss;
	task->trace = 0;
	task->io_map_addr = sizeof(struct TSS);
	task->ldtr = 0;
	task->cs = cs*8 + ring; task->eip = (unsigned int)ip;//-gdt[cs].base_l;
	task->ds = task->es = task->fs = task->gs = ds*8 + ring;
	task->ss = 8*((GDT_TASKS+1)+which*2) + ring; task->esp = STACKSIZE-4;
	task->ss0 = ss0*8; task->esp0 = sp0;
	task->eflags = 0x202L + (ring << 12);
	gdt[GDT_TASKS+which*2].limit=104;
	gdt[GDT_TASKS+which*2].base_l=(unsigned int)task;
	gdt[GDT_TASKS+which*2].base_m=0;
	gdt[GDT_TASKS+which*2].flags=ACS_TASK_STATE;
	gdt[GDT_TASKS+which*2].base_h=0;

	gdt[GDT_TASKS+1+which*2].limit=STACKSIZE;
	gdt[GDT_TASKS+1+which*2].base_l=(unsigned int)&tasks[which].stack;
	gdt[GDT_TASKS+1+which*2].base_m=0;
	gdt[GDT_TASKS+1+which*2].flags=ACS_DATA+(ring<<5);
	gdt[GDT_TASKS+1+which*2].base_h=0;
	
	if (rupt>=0)
	{
		idt[rupt].selector=8*(GDT_TASKS+which*2);
		idt[rupt].flags=0x85;
		idt[rupt].offset_high = idt[rupt].offset_low = idt[rupt].nothing = 0;
	}
	
}

void setup_tasks()
{
	setup_task(0, 0, kernel_code, kernel_data, 0, 0, 0, -1);
	setup_task(1, 3, tank_code, tank_data, madtank, spare_stack, STACKSIZE, -1);
	setup_task(2, 0, kernel_code, kernel_data, keyboard_task_loop, spare_stack, STACKSIZE,33);
	
}

void dump_mem(char * buf, int len)
{
	int i;
	for (i=0;i<len;i++)
		printc(buf[i]);
	printc('\n');
}

void scrcpy(char * dest, const char * src, int len)
{
	while(len--) {*dest++ = *src++;dest++;}
}

unsigned int histogram[256];

void do_histogram()
{
	int i;
	for (i=0;i<256;i++) histogram[i]=0;
	for (i=0;i<0x10000*TANKPAGES;i++)
		histogram[*(unsigned char*)(TANKAT+i)]++;
	clrscr();
	for (i=0;i<256;i++)
	{
		set_cursor(5*i);
		printc(' ');
		printn(histogram[i]);
	}
}

const char *tutorial3 = "MuOS Tutorial 3";
/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened!  All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */

int ticks;
unsigned int ip;
char * tockmsg="tock ";

//  for (i=0;i<999;i++) {i=i*2;i=i/2;}

int seen[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void delay()
{
	//return;
	int i;
	for (i=0;i<999999;i++) {i=i*2;i=i/2;}
}

void interrupt_handler(struct registers r)
{
	return;
    if (0) ;
    else if (r.int_no==33) //Keyboard
	    crash(); //IDT doesn't point here anymore, there's a task gate instead
    else if (r.int_no==32) //timer
    {
	    ticks = (ticks+1)%100;
	    if (1)//(!ticks)
	    {
			//print(tockmsg);
			//do_histogram();
			//Fry the offending instruction
			
			ip = r.eip-rand()%6 ;
			
			__asm__ (	"mov $0x20, %ax\n\t"
				"mov %ax, %fs\n\t"
				"mov ip, %edi\n\t"
				"call rand\n\t"
				"mov %al, %fs:(%edi)" );
			r.eip=r.esi=rand()%0xfff0;			
			//		r.eip=0;			
	    }
	    if (!ticks) do_histogram();
	    
    }
    else if (r.int_no < 32)
    {
		
		set_cursor(80*r.int_no);
		print(faultmsg[r.int_no]);
		if (!seen[r.int_no])
		{
			seen[r.int_no]=1;	
			delay();
		}
		//Fry the offending instruction
		ip = r.eip-rand()%6 ;
	    
		__asm__ (	"mov $0x20, %ax\n\t"
			"mov %ax, %fs\n\t"
			"mov ip, %edi\n\t"
			"call rand\n\t"
			"mov %al, %fs:(%edi)" );

        r.eip=r.esi=(rand()%0xfff0) ;//r.eip=tank_main;
    }
}

const char faultmsg[32][20] = 
{
	"Divide",	//ok
	"Debug", //ok
	"NMI",
	"Breakpoint", //ok
	"Overflow", //ok
	"Bounds", //ok
	"Unknown opcode", //ok
	"No math copro",
	"Double",
	"FP segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack", //ok
	"General protection", //ok
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

unsigned char in(unsigned short _port)
{
  // "=a" (result) means: put AL register in variable result when finished
  // "d" (_port) means: load EDX with _port
  unsigned char result;
  __asm__  ("in %%dx, %%al" : "=a" (result) : "d" (_port));
  return result;
}

void out(unsigned short _port, unsigned char _data)
{
  // "a" (_data) means: load EAX with _data
  // "d" (_port) means: load EDX with _port
  __asm__ ("out %%al, %%dx" : :"a" (_data), "d" (_port));
}

void clrscr()
{
  unsigned char *vidmem = (unsigned char *)0xB8000;
  const long size = 80*25;
  long loop;

  // Clear visible video memory
  for (loop=0; loop<size; loop++) {
    *vidmem++ = 0;
    *vidmem++ = 0xF;
  }

  // Set cursor position to 0,0
  out(0x3D4, 14);
  out(0x3D5, 0);
  out(0x3D4, 15);
  out(0x3D5, 0);
}

char printme[2]="-";
void printc(char c)
{
	printme[0]=c;
	print (printme);
}
void printn(int n)
{
	if (n>9) printn(n/10);
	printc('0'+n%10);
}

void printx(unsigned char n)
{
	unsigned char hi=n/16;
	if (hi>9)
		printc('A'+hi-10);
	else
		printc('0'+hi);
	hi=n%16;
	if (hi>9)
		printc('A'+hi-10);
	else
		printc('0'+hi);
	printc(' ');
}

void set_cursor(unsigned short offset)
{
  out(0x3D4, 15);
  out(0x3D5, (unsigned char)(offset));
  out(0x3D4, 14);
  out(0x3D5, (unsigned char)(offset >> 8));
}

void print(const char *_message)
{
  unsigned short offset;
  unsigned long i;
  unsigned char *vidmem = (unsigned char *)0xB8000;

  // Read cursor position
  out(0x3D4, 14);
  offset = in(0x3D5) << 8;
  out(0x3D4, 15);
  offset |= in(0x3D5);

  if (offset>24*80)
  {
	  set_cursor(0);
	  print(_message);
	  return;
  }
  // Start at writing at cursor position
  vidmem += offset*2;

  // Continue until we reach null character
  i = 0;
  while (_message[i] != 0) {
    *vidmem = _message[i++];
	vidmem += 2;
  }

  // Set new cursor position
  offset += i;
  out(0x3D5, (unsigned char)(offset));
  out(0x3D4, 14);
  out(0x3D5, (unsigned char)(offset >> 8));
}

void printfoo()
{
  print(foomsg);
}
const char * foomsg="Foo!";

void printbar()
{
  print(barmsg);

}
const char * barmsg="Bar!";

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

/* Handles the keyboard interrupt */

unsigned char scancode;

void keyboard_handler()
{
	//do_histogram();
	//return;
	/* Read from the keyboard's data buffer */
	scancode = in(0x60);
	if (scancode & 0x80)
	{
  		print("^");
	}
	else
	{
  		print("_");
		printc(kbdus[scancode]);
	}
}
