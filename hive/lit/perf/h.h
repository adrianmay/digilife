#include "types.h"

// The experiment code is in the test for now

typedef int PerfHandleS;
typedef int PerfHandleC;

typedef void (*PerfHandler)(PerfHandleC phc);

Cycles readCyclesNow(PerfHandleS phs);
PerfHandleS initProcPerf(PerfHandler ph, PerfHandleC phc); // All threads subsequently started by this one also included
PerfHandleS initThreadPerf(PerfHandler ph, PerfHandleC phc);
Cycles threadJustUsed(PerfHandleS phs); //Also adds to cpuCycles;
void arm(PerfHandleS phs, Cycles fromNow);
void disarm(PerfHandleS phs);

