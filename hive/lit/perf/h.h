#include "types.h"

// The experiment code is in the test for now

typedef int PerfHandleS;
typedef int PerfHandleC;

typedef void (*PerfHandler)(PerfHandleC phc);

Cycles readThreadCyclesNow(PerfHandleS phs);
PerfHandleS initThreadPerf(PerfHandler ph, PerfHandleC phc);
Cycles threadJustUsed(PerfHandleS phs); //Also adds to cpuCycles;
void arm(PerfHandleS phs, Cycles fromNow);

