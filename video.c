#include "zed.h"

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

void printn(int n)
{
	if (n>9) printn(n/10);
	printme[0]='0'+n%10;
	print (printme);
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

