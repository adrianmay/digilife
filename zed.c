struct registers
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

struct segment_descriptor {
  unsigned long 
	limit,
	base_l;
  unsigned short 
	base_m,
	access,
	attribs,
	base_h;
};

struct TSS {        /* TSS for 386+ */
	unsigned long 
		link,
		esp0,
		ss0,
		esp1,
		ss1,
		esp2,
		ss2,
		cr3,
		eip,
		eflags,
		eax,
		ecx,
		edx,
		ebx,
		esp,
		ebp,
		esi,
		edi,
		es,
		cs,
		ss,
		ds,
		fs,
		gs,
		ldtr;
	unsigned short  
		trace,
		io_map_addr;
} tss_kernel, tss_tank;


/* Access byte's flags */
#define ACS_PRESENT     0x80            /* present segment */
#define ACS_CSEG        0x18            /* code segment */
#define ACS_DSEG        0x10            /* data segment */
#define ACS_CONFORM     0x04            /* conforming segment */
#define ACS_READ        0x02            /* readable segment */
#define ACS_WRITE       0x02            /* writable segment */
#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define MAX_GDT 16
typedef enum {null, kernel_code, kernel_data, tank_code, tank_data, kernel_tss, tank_tss} gd_label;
extern struct segment_descriptor gdt[MAX_GDT];
extern unsigned char dp_cyl, dp_heads, dp_secpertrack;

unsigned char in(unsigned short _port);
void out(unsigned short _port, unsigned char _data);
void keyboard_handler();
void setup_gdt();
void jump_tank();
void tank_main();
void print(const char *_message);
void printc(char c);
void printx(unsigned char c);
void printfoo();
void printbar();
void isr_nothing();
void enable_A20();
void clrscr();
void setup_tasks();
void start_interrupts();
const char *tutorial3;
const char * foomsg;
const char * barmsg;
void interrupt_handler(struct registers *r);
void put_handler(unsigned int, void *, unsigned short int);//obsolete
#define GATE_DEFAULT 0x8E00

const char faultmsg[32][20];

void main()
{
	int a;
	setup_tasks();
	//put_handler(32, isr_nothing, GATE_DEFAULT);
	clrscr();
	printfoo();
	printx(dp_heads);
	printc(' ');
	printx(dp_cyl);
	printc(' ');
	printx(dp_secpertrack);
	printc(' ');
	jump_tank();
	
	for(;;);
}


void setup_tasks()
{
  tss_kernel.trace = tss_tank.trace = 0;
  tss_kernel.io_map_addr = 
         tss_tank.io_map_addr = sizeof(struct TSS);      /* I/O map just after the TSS */
  tss_kernel.ldtr = tss_tank.ldtr = 0;                /* ldtr = 0 */
  tss_tank.fs = tss_tank.gs = tss_tank.ds = tss_tank.es = tss_tank.ss = 0x10;      /* ds=es=ss = data segment */
  tss_tank.esp = 0x80000;    /* sp points to task stack top */
  tss_tank.cs = 8;
  tss_tank.eip = (unsigned short)&tank_main;                     /* cs:eip point to task() */
  tss_tank.eflags = 0x202L;                       /* interrupts are enabled */

	
}

void dump_mem(char * buf, int len)
{
	int i;
	for (i=0;i<len;i++)
		printc(buf[i]);
	printc('\n');
}


const char *tutorial3 = "MuOS Tutorial 3";
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
void keyboard_handler()
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = in(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        printc(kbdus[scancode]);
    }
}

//This didn't really go anywhere:
void put_gd (gd_label which, unsigned long base, unsigned long limit, unsigned char access, unsigned char attribs) 
{
	//return;
	struct segment_descriptor * item = &(gdt[which]);
	item->base_l = base & 0xFFFF;
	item->base_m = (base >> 16) & 0xFF;
	item->base_h = base >> 24;
	item->limit = limit & 0xFFFF;
	item->attribs = attribs | ((limit >> 16) & 0x0F);
	item->access = access;
}
void setup_gdt()
{
	put_gd(tank_code, 0, 0xffff, ACS_CODE, 0);
	return;
	put_gd(tank_data, 0, 0xffff, ACS_DATA, 0);
	dump_mem((unsigned char*)gdt, 3*8);
loop:
		goto loop;
}
