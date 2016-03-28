#include "headers.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"
#include <sys/stat.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <math.h>

double file_size(int file)
{
  struct stat fileStat;
  fstat(file, &fileStat);
  return fileStat.st_size;
};

int blocks_needed(double size)
{
  return ceil(size/DEFAULT_BLOCK_SIZE);
}

int main ( int argc, char *argv[] )
{
  char *disk_image;

  if(argc!= 3)
  {
    printf("Input Error. Usage: ./diskput <disk_image_file.img> <file.txt>\n");
    exit(-1);
  }

  disk_image = argv[1];
  int file=open(argv[2], O_RDONLY);

  double f_size = file_size(file);
  printf("File Size: \t%f bytes\n", f_size);
  long blocks_number =blocks_needed(f_size);
  printf("File Blocks: \t%ld blocks needed\n", blocks_number);

  close(file);
};
