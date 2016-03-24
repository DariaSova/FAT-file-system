#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "constants.h"
#include "fat.h"
#include "disk.h"


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

  struct Superblock *superblock;
  superblock = (struct Superblock*)malloc(sizeof(superblock));
  struct FAT *FAT;
  FAT = (struct FAT*)malloc(sizeof(FAT));
  struct FDirectory *root_directory;
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));


  printf("Super block information:\n");
  superblock->block_size = get_block_size(fp);
  printf("Block size: %d\n", superblock->block_size);
  superblock->blocks_num = get_blocks_num(fp);
  printf("Blocks number: %d\n", superblock->blocks_num);
  FAT->first_block = get_FAT_start(fp);
  printf("FAT starts: %d\n", FAT->first_block);
  FAT->blocks_num = get_FAT_blocks(fp);
  printf("FAT blocks: %d\n", FAT->blocks_num);
  root_directory->first_block = get_rootdir_start(fp);
  printf("Root Directory start: %d\n", root_directory->first_block);
  root_directory->blocks_num = get_rootdir_blocks(fp);
  printf("Root directory blocks: %d\n", root_directory->blocks_num);

  //READ FAT BLOCK
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  
  int free_blocks_counter = 0;
  int reserved_blocks_counter = 0;
  int allocated_blocks_counter = 0;

  printf("\nFAT information:\n");
  //for every entry in a FAT block
  for(int i= FAT->first_block; i<= FAT->blocks_num; i++)
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
