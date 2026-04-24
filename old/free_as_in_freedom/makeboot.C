#include <stdio.h>
#include <string.h>

int main(int argnr, char *args[])
{
  int total_sectors = -1;
  FILE *output, *input;
  int i, bytes_read, sectors, bytes_from_file;
  char buffer[512];

  if (argnr < 4) {
    printf("Invalid number of parameters.\n\n");
    printf("USAGE: %s [output] [input 1] [input 2] ... [input n]\n", args[0]);
    printf("Example: a.img bootsect.bin kernel.bin");
    return 1;
  }

  output = fopen(args[1], "w");

  for (i = 2; i < argnr; i++) {
    input = fopen(args[i], "r");

    if (input == NULL) {
      printf("Missing input file %s. Aborting operation...", args[i]);
      fclose(output);
      return 1;
    }

    bytes_read = 512;
    bytes_from_file = 0;
    sectors = 0;
    while(bytes_read == 512 && !feof(input)) {
      bytes_read = fread(buffer, 1, 512, input);

      if (bytes_read == 0)
        break;

      if (bytes_read != 512)
        memset(buffer+bytes_read, 0, 512-bytes_read);

      sectors++;
      fwrite(buffer, 1, 512, output);
      bytes_from_file += bytes_read;
    }

    printf("%d sectors, %d bytes read from file %s...\n", sectors, bytes_from_file, args[i]);
    total_sectors+=sectors;
    fclose(input);
  }
  memset(buffer, 0, 512);
  for (sectors=total_sectors+1;sectors<2880;sectors++)
  {
  	int leng;
  	fseek(output, sectors*512, SEEK_SET);
  	sprintf(buffer, "  Sector = %d    %n", sectors, &leng);
  	for (i=leng;i>=0;i--)
  		*((short*)(buffer+i*2))=buffer[i]+0x0f00;
  	fwrite(buffer, 1, 512, output);
  }
  fseek(output, 508, SEEK_SET);
  fwrite((char*)&total_sectors, 1, 1, output);
  fclose(output);
  return 0;
}
