#include "types.h"

// The experiment code is in the test for now

typedef int PerfHandleS;
typedef int PerfHandleC;

typedef void (*PerfHandler)(PerfHandleC phc);

void initPerf(); // Call this early in the whole process from the main thread
Cycles readProcessCycles();

PerfHandleS initThread(PerfHandler ph, PerfHandleC phc);
Cycles readThreadCycles(PerfHandleS phs);
void setAlarm(PerfHandleS phs, Cycles fromNow);
Cycles readAlarmCycles(PerfHandleS phs);
void cancelAlarm(PerfHandleS phs);

void nsToTs(uint64_t ns, struct timespec * pTs);
void sleepNs(uint64_t ns);

