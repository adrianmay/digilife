#include "types.h"

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b);
uint32_t wrapSub32U (uint32_t a, uint32_t b);
uint64_t wrapSub64U (uint64_t a, uint64_t b);
uint8_t  wrapAdd8  (uint8_t  a,  uint8_t b);
uint32_t wrapAdd32 (uint32_t a, uint32_t b);
uint64_t wrapAdd64 (uint64_t a, uint64_t b);
int8_t   wrapSub8S (uint8_t  a, uint8_t  b);

Nanosecs ageOfTime();
Nanosecs ageOfProcess();
Nanosecs ageOfThread();
pthread_t initTiming();
pthread_t sleepNs(Nanosecs * ns);
pthread_t sleepS(int s);
void wait(pthread_t);
void wake(pthread_t);
void sleepS_(int s) ;

#define TIME_VOID(CLOCK, WORK) \
{ Nanosecs start = CLOCK(); \
  WORK; \
  Nanosecs stop  = CLOCK(); \
  ns = stop - start; \
}

#define TIME_RET(CLOCK, RET, WORK) \
{ Nanosecs start = CLOCK(); \
  RET = WORK; \
  Nanosecs stop  = CLOCK(); \
  ns = stop - start; \
}

#define TIME_VOID_WALL(WORK)   TIME_VOID(ageOfTime, WORK)
#define TIME_VOID_PROC(WORK)   TIME_VOID(ageOfProcess, WORK)
#define TIME_VOID_THREAD(WORK) TIME_VOID(ageOfThread, WORK)
  
#define TIME_RET_WALL(RET, WORK)   TIME_RET(ageOfTime, RET, WORK)
#define TIME_RET_PROC(RET, WORK)   TIME_RET(ageOfProcess, RET, WORK)
#define TIME_RET_THREAD(RET, WORK) TIME_RET(ageOfThread, RET, WORK)
  
