#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "fat.h"
#include "constants.h"

void getName(FILE *fp, char *name)
{
  fread(name, 8, 1, fp);
}

uint32_t getBlockSize(FILE *fp)
{
  uint16_t size;
  fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
  fread(&size, 2, 1, fp);
  size = htons(size);
  return size;
};

uint32_t getBlocksNum(FILE *fp)
{
  uint32_t blocks_num;
  fseek(fp, BLOCKCOUNT_OFFSET, SEEK_SET);
  fread(&blocks_num, 4, 1, fp);
  blocks_num = ntohl(blocks_num);
  return blocks_num;
};

uint32_t getFATst(FILE *fp)
{
  uint32_t fat_starts;
  fseek(fp, FATSTART_OFFSET, SEEK_SET);
  fread(&fat_starts, 4, 1, fp);
  fat_starts = ntohl(fat_starts);
  return fat_starts;
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
  int blocks_num = 0;
  int fat_starts = 0;
  printf("Super block information:\n");
  getName(fp, system_name);
  printf("File system identifier: %s\n", system_name);
  size = getBlockSize(fp);
  printf("Block size: %d\n", size);
  blocks_num = getBlocksNum(fp);
  printf("Blocks number: %d\n", blocks_num);
  fat_starts = getFATst(fp);
  printf("FAT starts: %d\n", fat_starts);
  fclose(fp);
}
