void printbar();
const char *tutorial3;
void put_handler(unsigned int, void *, unsigned short int);
#define GATE_DEFAULT 0x8E00
void start()
{
  int a;
  clrscr();
  start_interrupts();
  put_handler(0,printbar,GATE_DEFAULT);
  print(tutorial3);
 // *((int*)0xffffffff)=1;
  a = 1/0;
  for(;;);//print(tutorial3);
}

const char *tutorial3 = "MuOS Tutorial 3";
