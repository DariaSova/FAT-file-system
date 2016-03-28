#include "headers.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"


int main ( int argc, char *argv[] )
{
  char *disk_image;

  if(argc != 3)
  {
    printf("Input Error. Usage: ./diskput <disk_image_file.img> <file.txt>\n");
    exit(-1);
  }

  disk_image = argv[1];
  FILE *fp;
  char *file = argv[2];

  fp = fopen(disk_image, "rb");
  fclose(fp);
};
