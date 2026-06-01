#include <time.h>
#include "types.h"

uint8_t  wrapAdd8   (uint8_t  a, uint8_t  b);
uint32_t wrapAdd32  (uint32_t a, uint32_t b);
uint64_t wrapAdd64  (uint64_t a, uint64_t b);

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b);
uint32_t wrapSub32U (uint32_t a, uint32_t b);
uint64_t wrapSub64U (uint64_t a, uint64_t b);

int8_t   wrapSub8S  (uint8_t  a, uint8_t  b);
int32_t  wrapSub32S (uint32_t a, uint32_t b);
int64_t  wrapSub64S (uint64_t a, uint64_t b);

int quit(int i);
int fileSize(int fd);

uint64_t randIntBelow(uint64_t lim);

bool nearly(double a, double b, double f);
