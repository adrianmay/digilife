

///>

#define GATE_DEFAULT 0x8E00
#define TANKAT 0x20000
#define TANKPAGES 1
#define STACKSIZE 2048
#define GDT_TASKS 6

///<

struct registers
{
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

typedef enum {null, 
	kernel_code, kernel_data, 
	tank_code, tank_data, 
	spare_stack_1, 
	kernel_tss, kernel_stack, 
	tank_tss, tank_stack, 
	keyboard_tss, keyboard_stack, 
	task_gate, screen, 
	spare_stack_2, GDT_MAX
} gd_label;


 struct gdt_descriptor {
	 unsigned short size;
	 void * p;
 };

 struct segment_descriptor {
	 unsigned short 	limit; 
	 unsigned short 	base_l;
	 unsigned char 	base_m;	
	 unsigned short 	flags;
	 unsigned char 	base_h;
 };

 struct idt_entry 
 {
	 unsigned short offset_low, selector;
	 unsigned char nothing, flags;
	 unsigned short offset_high;
 };

 struct TSS {        /* TSS for 386+ */
	 unsigned long 
		link,
		esp0,ss0,esp1,ss1,esp2,ss2,
		cr3,
		eip,
		eflags,
		eax,ecx,edx,ebx,
		esp,ebp,esi,edi,
		es,cs,ss,ds,fs,gs,
		ldtr;
 	unsigned short  
		trace,
		io_map_addr;
 };


 struct Task
 {
	 struct TSS tss;
	 unsigned char stack[STACKSIZE];
 };

///>

unsigned char in(unsigned short _port);
void out(unsigned short _port, unsigned char _data);
//void keyboard_handler();
void setup_gdt();
void jump_tank();
//void tank_main(); 
//void tank_idle();
void keyboard_task_loop();
void print(const char *_message);
void printn(int n);
void printc(char c);
void printx(int n);
//void isr_nothing();
//void enable_A20();
void setup_tasks();
//void start_interrupts();
const char *tutorial3;
const char * foomsg;
const char * barmsg;
void interrupt_handler(struct registers r);
//void put_handler(unsigned int, void *, unsigned short int);//obsolete
void load_tsr (unsigned int selector);
unsigned long rand(void);
void randinit();
void madtank();
void crash();
void delay();
void get_histogram(unsigned short int * histogram);
void do_histogram();
void pokescreen(int where, int what);
void clearscreen();
void at(int row, int col);
const char faultmsg[32][20];
extern int cursor;
extern struct gdt_descriptor gdt_desc;
extern struct segment_descriptor gdt[GDT_MAX];
extern struct Task tasks[3];
extern struct idt_entry idt[256];
extern const unsigned int TANK_START; 
extern const unsigned int KERNEL_CODE_END; 
extern const unsigned int KERNEL_DATA_BEGIN; 
extern const unsigned int KERNEL_END; 



