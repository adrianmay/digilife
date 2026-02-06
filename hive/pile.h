// This file must be included by one that instantiates the macros in pile.m4, and by pile.c

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#define BAD_INDEX UINT32_MAX
typedef const char * Str;

typedef struct { // This should be of a good size for alignment
  int fd; // File descriptor for underlying file
  uint32_t res; // File has space for this many records
  uint32_t top; // Index of first untouched record
  uint32_t fre; // Index of free record containing a pointer to the next one
} Pilehead;

// Pass filename, record length, something unused, how many records to make room for when growing, max records
void * openPileInternal(Str filename, uint32_t rec, uint32_t preamble, uint32_t stp, uint64_t lim);

// Alloc a record stuffing it with prototype, stuffing ghost with the former contents of the free block
uint32_t  allocInternal(Pilehead * ph, uint32_t rec, uint32_t preamble, uint32_t stp, char * prototype, void * ghost, int ghostlen);

// Free a record, inserting ghost into the freed block
void   freeInternal (Pilehead * ph, uint32_t i, uint32_t rec, uint32_t preamble, void * ghost, int ghostlen);

uint32_t countFree(Pilehead * ph, uint32_t rec);

