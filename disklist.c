#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "constants.h"

int main ( int argc, char *argv[] )
{
  char *disk_image;

  if(argc != 2)
  {
    printf("Input Error. Usage: ./diskinfo <disk_image_file.img>\n");
    exit(-1);
  }

  disk_image = argv[1];
  //read superblock
  FILE *fp;

  fp = fopen(disk_image, "rb");

  fclose(fp);
};
