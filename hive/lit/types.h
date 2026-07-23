#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

#define MAX_WORKER_THREADS 100 // These are file handles so some are wasted
#define NS_PER_SEC 1000000000.0
#define SECS_PER_MIN 60.0
#define MINS_PER_HOUR 60.0
#define SECS_PER_HOUR (SECS_PER_MIN*MINS_PER_HOUR)
#define HOURS_PER_DAY 24.0
#define DAYS_PER_YEAR 365.25
#define SECS_PER_YEAR (DAYS_PER_YEAR*HOURS_PER_DAY*MINS_PER_HOUR*SECS_PER_MIN)
#define SECS_PER_MONTH (SECS_PER_YEAR/12.0)

#define BAD_INDEX UINT32_MAX
#define GOD_BOMB 0xfffffffe
#define MAX_FILENAME 256
#define PAGE 4096
#define KILO 1024ull
#define MEGA (KILO*KILO)
#define GIGA (MEGA*KILO)
#define B8 256ull
#define B16 (B8*B8)
#define B32 (B16*B16)

#define NICK_NAME_GOD    0x10000000
// Todo: extend this when other stuff done:
#define NICK_NAME_RAND_MASK 0x0FFFFFFF
#define MOB_GROSS_SIZE  64
#define MOB_HEADER_SIZE 24
#define MOB_BODY_SIZE   40

// Ought to be using the FPU here
typedef uint64_t Cycles;
typedef int64_t CycleDiff;
typedef uint32_t Tocks;  // A tock might be around a microsecond, but varies
typedef int32_t  TockDiff;  // A tock might be around a microsecond, but varies
typedef Tocks    Score;
typedef TockDiff ScoreDiff;
typedef uint32_t Ix;
typedef  int32_t IxDiff;
//typedef uint32_t Name;
typedef Ix Nick;
typedef uint32_t TockDuration; // In nanoseconds.
typedef int64_t Cash;
typedef double TockPrice; // The currency unit can be tiny.
typedef int64_t Weight; // We use negative ones when removing stuff. TODO: 32 bit
// These are the self and subtree weights. The total weight is the sum of these:
typedef struct {Weight s; Weight l; Weight r;} Weights;
typedef double CpuBid; // Float or int?

typedef void   (*V)(void);
typedef bool   (*B)(void);
typedef void   (*V_C)(Cash);
typedef void   (*V_P)(void *);
typedef void   (*V_P_P)(void *, void *);
typedef void * (*P_P_P)(void *, void *);
typedef void   (*V_I_P)(Ix, void *);

typedef enum {Nowt, Delete, Hide} Fate;
typedef enum {Exact, Ono, Rob} Terms;
typedef enum {Ok, Dead, Busy} Woth;


extern int iter;

