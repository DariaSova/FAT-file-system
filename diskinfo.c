#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "constants.h"

void get_name(FILE *fp, unsigned char *name)
{
  fread(name, 8, 1, fp);
}

uint32_t get_block_size(FILE *fp)
{
  uint16_t size;
  fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
  fread(&size, 2, 1, fp);
  return htons(size);
};

uint32_t get_blocks_num(FILE *fp)
{
  uint32_t blocks_num;
  fseek(fp, BLOCKCOUNT_OFFSET, SEEK_SET);
  fread(&blocks_num, 4, 1, fp);
  return ntohl(blocks_num);
};

uint32_t get_FAT_start(FILE *fp)
{
  uint32_t fat_starts;
  fseek(fp, FATSTART_OFFSET, SEEK_SET);
  fread(&fat_starts, 4, 1, fp);
  return ntohl(fat_starts);
};

uint32_t get_FAT_blocks(FILE *fp)
{
  uint32_t fat;
  fseek(fp, FATBLOCKS_OFFSET, SEEK_SET);
  fread(&fat, 4, 1, fp);
  return ntohl(fat);
};

uint32_t get_rootdir_start(FILE *fp)
{
  uint32_t dir_start;
  fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
  fread(&dir_start, 4, 1, fp);
  return ntohl(dir_start);
};

uint32_t get_rootdir_blocks(FILE *fp)
{
  uint32_t root_dir;
  fseek(fp, ROOTDIRBLOCKS_OFFSET, SEEK_SET);
  fread(&root_dir, 4, 1, fp);
  return ntohl(root_dir);
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

  fp = fopen(disk_image, "rb");

  uint32_t size = 0;
  uint32_t blocks_num = 0;
  uint32_t fat_starts = 0;
  uint32_t fat_blocks = 0;
  uint32_t root_dir_blocks = 0;
  uint32_t root_dir_start = 0;

  printf("Super block information:\n");
  size = get_block_size(fp);
  printf("Block size: %d\n", size);
  blocks_num = get_blocks_num(fp);
  printf("Blocks number: %d\n", blocks_num);
  fat_starts = get_FAT_start(fp);
  printf("FAT starts: %d\n", fat_starts);
  fat_blocks = get_FAT_blocks(fp);
  printf("FAT blocks: %d\n", fat_blocks);
  root_dir_start = get_rootdir_start(fp);
  printf("Root Directory start: %d\n", root_dir_start);
  root_dir_blocks = get_rootdir_blocks(fp);
  printf("Root directory blocks: %d\n", root_dir_blocks);

  //READ FAT BLOCK
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  
  int free_blocks_counter = 0;
  int reserved_blocks_counter = 0;
  int allocated_blocks_counter = 0;

  printf("\nFAT information:\n");
  //for every entry in a FAT block
  for(int i= fat_starts; i<= fat_blocks; i++)
  {
    fseek(fp, i*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    for(int j=0; j< DEFAULT_BLOCK_SIZE/FAT_ENTRY_SIZE; j++)
    {
      uint32_t* entry = (uint32_t*)&buf[pointer];
      uint32_t result = ntohl(*entry);

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
  }
  printf("Free Blocks: %d\n", free_blocks_counter);
  printf("Reserved Blocks: %d\n", reserved_blocks_counter);
  printf("Allocated Blocks: %d\n", allocated_blocks_counter);

  fclose(fp);
}
