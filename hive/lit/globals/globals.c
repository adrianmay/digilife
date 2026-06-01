#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "structs.h"
#include "h.h"
#include "perf/h.h"
#include "misc/h.h"

#define GUESS_CYCLES_PER_TOCK 1000000
#define GLOBALS_FILENAME "Globals.pile"

VolatileGlobals vg;
PersistentGlobals * pg;

static void initVirginPersistentGlobals(void) {
  pg->lastKnownTock = 0;
  pg->cyclesNotTocked = 0;
  pg->cyclesPerTock = GUESS_CYCLES_PER_TOCK; //Don't ignore animal for more than 2**32/nsPerTock
  pg->groatsPerTockPerByte = 0.014; //min_groats_per_nanosecond * GUESS_NS_PER_TOCK;
}

static void initVolatileGlobals(void) {
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
  if (fd<0) { fprintf(stdout, "Can't open file %s cos of %d\n", GLOBALS_FILENAME, fd); quit(1); }
  void * reserve = mmap(0, len, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if (reserve == (void*)-1) { fprintf(stdout, "Can't make reserve mapping\n"); quit(1); }
  void * filemap = mmap(reserve, fileSize(fd), PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, fd, 0);
  if (filemap == (void*)-1) { fprintf(stdout, "Can't make file mapping\n"); quit(1);  }
  return filemap;
}

// Close and maybe delete the file
static void closeGlobals_(int fd, FATE fate) {
  if (fd == -1) return;
  //munmap(ph); //TODO: I thought I needed this
  close(fd);
  if (fate==DELETE) unlink(GLOBALS_FILENAME);
  if (fate==HIDE) {
    char dest[MAX_FILENAME+1];
    *dest='.';
    strcpy(dest+1,GLOBALS_FILENAME);
    rename(GLOBALS_FILENAME, dest);
  }
}

bool openGlobals(void) {
  bool v;
  pg = (PersistentGlobals *) openGlobals_(sizeof(PersistentGlobals), &v);
  if (v) initVirginPersistentGlobals();
  initVolatileGlobals();
  return v;
}

void closeGlobals(bool rm) {  // And that param should be enum
  closeGlobals_(pg->fd, rm);
}

TockPrice tockPrice(void) {return pg->groatsPerTockPerByte;}

void updateTocks(void) {
  Cycles now = readProcessCycles();
  Cycles sleptFor = wrapSub64U(now, vg.tocksReviewedAt);
  Cycles toBill = sleptFor + pg->cyclesNotTocked;
  vg.tocksReviewedAt = now;
  lldiv_t qr = lldiv(toBill, pg->cyclesPerTock);
  pg->lastKnownTock = pg->lastKnownTock + qr.quot;
  pg->cyclesNotTocked = qr.rem;
}

Tocks tocksNow(void) {return pg->lastKnownTock;}
Cycles cyclesUntilTock(Tocks deadline) {return (deadline - pg->lastKnownTock)*pg->cyclesPerTock - pg->cyclesNotTocked;}
Cycles cyclesAtTock(Tocks deadline) {return cyclesUntilTock(deadline)+readProcessCycles();}

