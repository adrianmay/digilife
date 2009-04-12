struct registers
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

unsigned char in(unsigned short _port);
void out(unsigned short _port, unsigned char _data);

void print(const char *_message);
void printfoo();
void printbar();
void isr_nothing();
void enable_A20();
void clrscr();
void start_interrupts();
const char *tutorial3;
const char * foomsg;
const char * barmsg;
void finterrupt_handler(struct registers *r);
void put_handler(unsigned int, void *, unsigned short int);//obsolete
#define GATE_DEFAULT 0x8E00

const char faultmsg[32][20];


