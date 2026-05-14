// The header for a pile:
struct __attribute__((aligned(KILO))) Pilehead { // This should be of a good size for alignment
  Ix hdr; // Size of this header
  int fd; // File descriptor for mapped file
  Ix rec; // Size of records
  Ix stp; // How many 4k pages to add when growing
  Ix lim; // Max pages
  Ix res; // File has space for this many records
  Ix top; // Ix of first untouched record, i.e., high watermark
  Ix fri; // Ix of recently freed record
  Ix fro; // Ix of next free record to be used in alloc
  Ix frn; // Num free slots
  char fn[MAX_FILENAME]; // So we can offer to delete it upon closing           
  Ix usr; // Misc persistent variable about this pile.TODO: make it atomic
};
                            

