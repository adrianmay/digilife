// core/structs.h

typedef struct {
  CoreHandle handle;
  bool quitting;
  bool forceYield;
  Alarm alarm;
} Core;
