#include "types.h"
#include <time.h>

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b);
uint32_t wrapSub32U (uint32_t a, uint32_t b);
uint64_t wrapSub64U (uint64_t a, uint64_t b);
uint8_t  wrapAdd8   (uint8_t  a, uint8_t  b);
uint32_t wrapAdd32  (uint32_t a, uint32_t b);
uint64_t wrapAdd64  (uint64_t a, uint64_t b);
int8_t   wrapSub8S  (uint8_t  a, uint8_t  b);

Nanosecs ageOfTime();
Nanosecs ageOfProcess();
Nanosecs ageOfThread();

int quit(int i);
int fileSize(int fd);                                                         

void nsToTs(Nanosecs ns, struct timespec * pTs);
int sleepNs(Nanosecs ns);
