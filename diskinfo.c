#include "headers.h"
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

  superblock = (struct Superblock*)malloc(sizeof(superblock));
  FAT = (struct FAT*)malloc(sizeof(FAT));
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

  read_FAT(fp);
  printf("\nFAT information:\n");
  printf("Free Blocks: %d\n", FAT->free_blocks);
  printf("Reserved Blocks: %d\n", FAT->reserved_blocks);
  printf("Allocated Blocks: %d\n", FAT->allocated_blocks);

  fclose(fp);
}
