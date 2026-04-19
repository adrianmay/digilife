#include <sys/stat.h>                                
#include <stdio.h>
#include <stdlib.h>         
#include "types.h"         

int quit(int i) {abort();} // { return *((int*)(0)); }                         
    
int fileSize(int fd) {                                                         
  struct stat sb;                                                              
  if (fstat(fd, &sb) == -1) { printf("Can't stat fd=%d\n", fd); quit(1); }
  return sb.st_size;
} 
  
