#include <stdint.h>         
#include <stdbool.h>
#include <stdlib.h>         

#define BAD_INDEX UINT32_MAX
#define MAX_FILENAME 256
#define PAGE 4096    
#define KILO 1024           
#define MEGA (KILO*KILO)    
#define GIGA (MEGA*KILO)    
#define TERA (GIGA*KILO)    
#define B8 256ull           
#define B16 (B8*B8)         
#define B32 (B16*B16)       

// Ought to be using the FPU here
typedef uint64_t Nanosecs;
typedef uint32_t Tocks;  // A tock might be around a microsecond, but varies
typedef uint32_t Index;
typedef uint32_t TockDuration; // In nanoseconds.
typedef uint64_t Score;
typedef uint64_t Cash;  
typedef uint32_t TockPrice; // The currency unit can be tiny.


