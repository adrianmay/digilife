//This is types.h

#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include <stdint.h>         
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

#define NS_PER_SEC 1000000000.0
#define SECS_PER_MIN 60.0
#define MINS_PER_HOUR 60.0
#define SECS_PER_HOUR (SECS_PER_MIN*MINS_PER_HOUR)
#define HOURS_PER_DAY 24.0
#define DAYS_PER_YEAR 365.25
#define SECS_PER_YEAR (DAYS_PER_YEAR*HOURS_PER_DAY*MINS_PER_HOUR*SECS_PER_MIN)
#define SECS_PER_MONTH (SECS_PER_YEAR/12.0)

#define BAD_INDEX UINT32_MAX
#define MAX_FILENAME 256
#define PAGE 4096    
#define KILO 1024ull           
#define MEGA (KILO*KILO)    
#define GIGA (MEGA*KILO)    
#define B8 256ull           
#define B16 (B8*B8)         
#define B32 (B16*B16)       

#define LIKE_FREE 2
// Ought to be using the FPU here
typedef uint64_t Nanosecs;
typedef uint32_t Tocks;  // A tock might be around a microsecond, but varies
typedef int32_t  TockDiff;  // A tock might be around a microsecond, but varies
typedef Tocks    Score;
typedef TockDiff ScoreDiff;
typedef uint32_t Index;
typedef  int32_t IndexDiff;
typedef uint32_t Name;
typedef uint32_t TockDuration; // In nanoseconds.
typedef int64_t Cash;  
typedef double TockPrice; // The currency unit can be tiny.
typedef int64_t Weight; // We use negative ones when removing stuff
typedef double CpuBid; // Float or int?

typedef void * (*F)(void * item, void * u);
typedef void (*V)();

typedef enum {NOWT, DELETE, HIDE} FATE;

#endif

