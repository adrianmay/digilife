// The header for a pile:
struct __attribute__((aligned(KILO))) Pilehead { // This should be of a good size for alignment
  Index hdr; // Size of this header
  int fd; // File descriptor for mapped file
  Index rec; // Size of records
  Index stp; // How many 4k pages to add when growing
  Index lim; // Max pages
  Index res; // File has space for this many records
  Index top; // Index of first untouched record, i.e., high watermark
  Index fri; // Index of recently freed record
  Index fro; // Index of next free record to be used in alloc
  _Atomic Index frn; // Num free slots
  char fn[MAX_FILENAME]; // So we can offer to delete it upon closing           
  Index usr; // Misc persistent variable about this pile.
};
                            

