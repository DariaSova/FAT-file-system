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

uint32_t getFATblocks(FILE *fp)
{
  uint32_t fat;
  fseek(fp, FATBLOCKS_OFFSET, SEEK_SET);
  fread(&fat, 4, 1, fp);
  fat = ntohl(fat);
  return fat;
};

uint32_t getRootDirStart(FILE *fp)
{
  uint32_t dir_start;
  fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
  fread(&dir_start, 4, 1, fp);
  dir_start = ntohl(dir_start);
  return dir_start;
};

uint32_t getRootDirBlocks(FILE *fp)
{
  uint32_t root_dir;
  fseek(fp, ROOTDIRBLOCKS_OFFSET, SEEK_SET);
  fread(&root_dir, 4, 1, fp);
  root_dir = ntohl(root_dir);
  return root_dir;
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

  char system_name[30];
  int size = 0;
  int blocks_num = 0;
  int fat_starts = 0;
  int fat_blocks = 0;
  int root_dir_blocks = 0;
  int root_dir_start = 0;

  printf("Super block information:\n");
  getName(fp, system_name);
  printf("File system identifier: %s\n", system_name);
  size = getBlockSize(fp);
  printf("Block size: %d\n", size);
  blocks_num = getBlocksNum(fp);
  printf("Blocks number: %d\n", blocks_num);
  fat_starts = getFATst(fp);
  printf("FAT starts: %d\n", fat_starts);
  fat_blocks = getFATblocks(fp);
  printf("FAT blocks: %d\n", fat_blocks);
  root_dir_start = getRootDirStart(fp);
  printf("Root Directory start: %d\n", root_dir_start);
  root_dir_blocks = getRootDirBlocks(fp);
  printf("Root directory blocks: %d\n", root_dir_blocks);

  //READ FAT BLOCK
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  fseek(fp, DEFAULT_BLOCK_SIZE, SEEK_SET);
  fread(buf, 512, 1, fp);
  
  int free_blocks_counter = 0;
  int reserved_blocks_counter = 0;
  int allocated_blocks_counter = 0;

  //for every entry in a FAT block
  ///for(int i=0; i< fat_blocks; i++)
  //{
    int pointer = 0;
    for(int j=0; j< DEFAULT_BLOCK_SIZE/FAT_ENTRY_SIZE; j++)
    {
      uint32_t* entry = (uint32_t*)&buf[pointer]; //fat entry size
      uint32_t result = ntohl(*entry);
      //printf("Entry: %08X\n", result);

      switch(result)
      {
        case FAT_FREE:
          free_blocks_counter++;
          break;
        case FAT_RESERVED:
          reserved_blocks_counter++;
          break;
        default:
          allocated_blocks_counter++;
        break;
      }
      pointer+=FAT_ENTRY_SIZE;
    }
    printf("FREE: %d\n", free_blocks_counter);

  //}

  fclose(fp);
}
