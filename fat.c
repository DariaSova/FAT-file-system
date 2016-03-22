#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "fat.h"
#include "constants.h"

void getName(FILE *fp, char *name)
{
  fread(name, 8, 1, fp);
}

int getBlockSize(FILE *fp)
{
  //Little Endian case
  int retVal;
  int *high = malloc(sizeof(int));
  int *low = malloc(sizeof(int));
  fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
  fread(high, 1, 1, fp);
  fread(low, 1, 1, fp);
  retVal = ((*high)<<8) + *low;
  free(high);
  free(low);
  return retVal;
};

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
  struct Superblock *superblock;
  superblock = (struct Superblock*)malloc(sizeof(superblock));

  fp = fopen(disk_image, "rb");

  //char buf[DEAFAULT_BLOCK_SIZE];
  //char pointer = buf[0];

  /*//read superblock
  fseek(fp, 0, SEEK_SET);
  fread(buf, 512, 1, fp);
  fread(buf2, 2, 1, fp);
  //printf("%d\n", buf2);
  //superblock->system_id = buf;
  printf("SIZE: %d\n", superblock->system_id);
*/
  char system_name[30];
  int size = 0;
  printf("Super block information:\n");
  getName(fp, system_name);
  printf("File system identifier: %s\n", system_name);
  size = getBlockSize(fp);
  printf("Block size: %d\n", size);
  fclose(fp);
}
