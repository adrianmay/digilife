#include "types.h"

#define SET 1
#define QUIT 2
#define WAIT 4

typedef bool (*Worker)(YYIndex iYY, Nanosecs * pNsRel); // Returns SET
typedef int  (*Looper)(Nanosecs * pNsRel); // Returns the above flags

void blockXXTimerSignal();
void unblockXXTimerSignal();
void initXXTimer();
void workOnXXTimer(Worker worker, YYIndex iYY);               //Works within the mutex and optionally sets the timer.
void loopOnXXTimer(Looper looper, Nanosecs max); //Works within the mutex and optionally quits, sets the timer, and/or waits on it.
void lockXXTimer(bool lock);
void unitXXTimer();
