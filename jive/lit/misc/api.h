#include <time.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

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
int die(const char * file, int line, const char *fmt, ...);
#define DIE(WHY, ...) die(__FILE__, __LINE__, WHY, ##__VA_ARGS__)

int fileSize(int fd);

uint32_t randInt32Masked(uint32_t mask);
uint64_t randIntBelow(uint64_t lim);

bool nearly(double a, double b, double f);
double gaussian_random(double mean, double stddev);
double clampProb(double p);

void nsToTs(uint64_t ns, struct timespec * pTs);
void sleepNs(uint64_t ns);
void sleepMs(uint64_t ms);
