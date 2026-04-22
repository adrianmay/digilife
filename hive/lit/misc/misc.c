#include <sys/stat.h>                                
#include <stdio.h>
#include <stdlib.h>         
#include "types.h"         

////////////////////////////////////////////////////////////////

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b) { return a - b; }
uint32_t wrapSub32U (uint32_t a, uint32_t b) { return a - b; }
uint64_t wrapSub64U (uint64_t a, uint64_t b) { return a - b; }
uint8_t  wrapAdd8  (uint8_t  a,  uint8_t b) { return a + b; }
uint32_t wrapAdd32 (uint32_t a, uint32_t b) { return a + b; }
uint64_t wrapAdd64 (uint64_t a, uint64_t b) { return a + b; }
int8_t   wrapSub8S (uint8_t  a, uint8_t  b) { return a - b; }

////////////////////////////////////////////////////////////////

Nanosecs age(clockid_t what) {
  struct timespec ts;
  clock_gettime(what, &ts); 
  return ts.tv_sec*1000000000 + ts.tv_nsec;
}

Nanosecs ageOfTime()    { return age(CLOCK_REALTIME); }
Nanosecs ageOfProcess() { return age(CLOCK_PROCESS_CPUTIME_ID); }
Nanosecs ageOfThread()  { return age(CLOCK_THREAD_CPUTIME_ID); }

////////////////////////////////////////////////////////////////

int quit(int i) {abort();} // { return *((int*)(0)); }                         
    
int fileSize(int fd) {                                                         
  struct stat sb;                                                              
  if (fstat(fd, &sb) == -1) { printf("Can't stat fd=%d\n", fd); quit(1); }
  return sb.st_size;
} 
  
