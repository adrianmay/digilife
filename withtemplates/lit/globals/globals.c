#include <fcntl.h>                                   
#include <string.h>
#include <sys/mman.h>                                
#include <unistd.h>
#include "types.h"
#include "h.h"
#include "misc/h.h"

#define GUESS_NS_PER_TOCK 1000000                                           
#define GLOBALS_FILENAME "Globals.pile"

typedef struct {
  Nanosecs tocksReviewedAt; // Exact CPU uptime 
  bool shouldRun;                          
} VolatileGlobals;

typedef struct __attribute__((aligned(KILO))) { 
  int fd; // For globals
  Tocks lastKnownTock; // 
  Nanosecs nsNotTocked; // Tocks were rounded down, such that this much time was not charged
  TockDuration nsPerTock; // Easy way to adjust price per ns, while keeping price per tock fixed.
  TockPrice groatsPerTock;                       
} PersistentGlobals;

VolatileGlobals vg; 
PersistentGlobals * pg; 

static void initVirginPersistentGlobals() {
  pg->lastKnownTock = 0;
  pg->nsNotTocked = 0;
  pg->nsPerTock = GUESS_NS_PER_TOCK; //Don't ignore animal for more than 2**32/nsPerTock
  pg->groatsPerTock = 1; //min_groats_per_nanosecond * GUESS_NS_PER_TOCK;
}

static void initVolatileGlobals() {
  vg.tocksReviewedAt = 0;
  vg.shouldRun = true;
}

static void * openGlobals_(uint64_t len, bool * virgin) {
  *virgin=false;
  int fd = open(GLOBALS_FILENAME, O_RDWR | O_APPEND);
  if (fd<0) {
    *virgin=true;
    fd = open(GLOBALS_FILENAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR|S_IWUSR);
    ftruncate(fd, len);
  }
  if (fd<0) { fprintf(stderr, "Can't open file %s cos of %d\n", GLOBALS_FILENAME, fd); quit(1); }
  void * reserve = mmap(0, len, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if (reserve == (void*)-1) { fprintf(stderr, "Can't make reserve mapping\n"); quit(1); }
  void * filemap = mmap(reserve, fileSize(fd), PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, fd, 0);
  if (filemap == (void*)-1) { fprintf(stderr, "Can't make file mapping\n"); quit(1);  }
  return filemap;
}

// Close and maybe delete the file
static void closeGlobals_(int fd, bool rm) {
  if (fd == -1) return;
  //munmap(ph); //TODO: I thought I needed this
  close(fd);
  if (rm) unlink(GLOBALS_FILENAME);
}

bool openGlobals() {
  bool v;
  pg = (PersistentGlobals *) openGlobals_(sizeof(PersistentGlobals), &v);
  if (v) initVirginPersistentGlobals();
  initVolatileGlobals();
  return v;
}

void closeGlobals(bool rm) {  // And that param should be enum
  closeGlobals_(pg->fd, rm); 
} 

