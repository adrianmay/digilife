//This is types.h
#include <pthread.h>
#include <stdint.h>         
#include <stdbool.h>
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
typedef uint32_t Index;
typedef uint32_t TockDuration; // In nanoseconds.
typedef uint32_t Score;
typedef uint64_t Cash;  
typedef double TockPrice; // The currency unit can be tiny.

typedef void * (*F)(void * item, void * u);

