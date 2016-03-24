#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "constants.h"
#include "fat.h"

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

  struct Superblock *superblock;
  superblock = (struct Superblock*)malloc(sizeof(superblock));
  struct FAT *FAT;
  FAT = (struct FAT*)malloc(sizeof(FAT));
  struct FDirectory *root_directory;
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));

  uint32_t size = 0;
  uint32_t blocks_num = 0;
  uint32_t fat_starts = 0;
  uint32_t fat_blocks = 0;
  uint32_t root_dir_blocks = 0;
  uint32_t root_dir_start = 0;

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


  int counter =0;
  ///DIRECTORY
  for(int i=root_dir_start; i< root_dir_start+root_dir_blocks; i++)
  {
    fseek(fp, i*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    for(int j=0; j< 8; j++)
    {
      counter++;
      uint8_t* mask = (uint8_t*)&buf[pointer]; 
      pointer++;

      if((*mask&DIRECTORY_ENTRY_USED)==DIRECTORY_ENTRY_USED)
      {
        if((*mask&DIRECTORY_ENTRY_FILE)==DIRECTORY_ENTRY_FILE)
        {
          printf("F ");
        }
        else if((*mask&DIRECTORY_ENTRY_DIRECTORY)==DIRECTORY_ENTRY_DIRECTORY)
        {
          printf("D ");
        }
        //skip first block info
        pointer+=4;
        pointer+=4;
        uint32_t* file_size = (uint32_t*)&buf[pointer];
        pointer+=4;
        //skip creation date
        pointer+=7;
        uint16_t* year = (uint16_t*)&buf[pointer]; 
        pointer+=2;
        uint8_t* month = (uint8_t*)&buf[pointer]; 
        pointer++;
        uint8_t* day = (uint8_t*)&buf[pointer]; 
        pointer++;
        uint8_t* hour = (uint8_t*)&buf[pointer]; 
        pointer++;
        uint8_t* minute = (uint8_t*)&buf[pointer]; 
        pointer++;
        uint8_t* seconds = (uint8_t*)&buf[pointer]; 
        pointer++;
        //read file name
        unsigned char file_name [30];
        for(int k=0; k<31; k++)
          file_name[k] = buf[pointer++];
        pointer+=6;

        printf("%10d ", ntohl(*file_size));
        printf("%30s ", file_name);
        printf("%d/%02X/%02X %i:%i:%02i\n", htons(*year), *month, *day, *hour, *minute, *seconds);
      }
    }
  }

  fclose(fp);
}
