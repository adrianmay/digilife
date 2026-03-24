#include <stdint.h>         
#include <stdbool.h>

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
typedef uint32_t Tocks;  // A tock might be around a microsecond, but varies
typedef uint32_t Index;
typedef uint32_t TockDuration; // In nanoseconds.
typedef uint64_t Score;
typedef uint64_t Cash;  
typedef uint32_t TockPrice; // The currency unit can be tiny.

typedef struct __attribute__((aligned(KILO))) { 
  int fd; // For globals
  uint64_t tocksReviewedAt; // Exact CPU uptime 
  Tocks lastKnownTock; // 
  uint64_t nsNotTocked; // Tocks were rounded down, such that this much time was not charged
  TockDuration nsPerTock; // Easy way to adjust price per ns, while keeping price per tock fixed.
  TockPrice groatsPerTock;                       
} Globals;

extern Globals * g;

void initDefaultGlobals();

