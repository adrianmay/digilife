#include "types.h"
#include <pthread.h>

uint8_t  wrapSubtract8 (uint8_t  a, uint8_t  b);
uint32_t wrapSubtract32(uint32_t a, uint32_t b);
uint64_t wrapSubtract64(uint64_t a, uint64_t b);
uint8_t  wrapAdd8      (uint8_t  a,  uint8_t b);
uint32_t wrapAdd32     (uint32_t a, uint32_t b);
uint64_t wrapAdd64     (uint64_t a, uint64_t b);

Nanosecs ageOfProcess();
Nanosecs ageOfThread();
pthread_t initTiming();
void sleepNs(Nanosecs ns);
void sleepS(int ns);
void wake(pthread_t);

