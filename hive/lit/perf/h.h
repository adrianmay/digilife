#include "types.h"

// The experiment code is in the test for now

typedef int MyPerfHandle;
typedef int YourPerfHandle;

typedef void (*PerfHandler)(YourPerfHandle yph);

MyPerfHandle initThreadPerf(PerfHandler ph, YourPerfHandle yph);
Cycles threadJustUsed(MyPerfHandle mph); //Also adds to cpuCycles;
void arm(MyPerfHandle mph, Cycles fromNow);

