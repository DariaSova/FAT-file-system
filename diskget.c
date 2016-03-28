#include "headers.h"
#include "constants.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"

void copy_file(FILE *fp, char* file_name, struct FDirectory* file)
{
  FILE* new_file = fopen(file_name, "wb");
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  unsigned char fat_buf[4];

  int full_blocks_num = file->size/DEFAULT_BLOCK_SIZE;
  int rem_bytes = file->size%DEFAULT_BLOCK_SIZE;
  uint32_t current_block;
  current_block = file->first_block;
  uint8_t* current_byte; 


  for(int i=0; i<full_blocks_num; i++)
  {

    fseek(fp, (long)((current_block)*DEFAULT_BLOCK_SIZE), SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    for(int j=0; j<512; j++)
    {
      current_byte = (uint8_t*)&buf[pointer]; 
      pointer++;
      fwrite(current_byte, 1, 1, new_file);
    }

    if(i==(file->blocks_num-1))
      printf("Done.\n");
    else
    { 
      fseek(fp, (long)1*DEFAULT_BLOCK_SIZE+current_block*4, SEEK_SET);
      fread(fat_buf, 4, 1, fp);
      uint32_t* next = (uint32_t*)&fat_buf[0];
      current_block = ntohl(*next);
    }
  }

  //copy remaining bytes
  if(rem_bytes>0)
  {
    fseek(fp, (long)(current_block)*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    for(int j=0; j<rem_bytes; j++)
    {
      current_byte = (uint8_t*)&buf[j]; 
      fwrite(current_byte, 1, 1, new_file);
    }
  }
  fclose(new_file);
};

void read_root_directory(FILE *fp, struct FDirectory* root_directory, struct FDirectory* file, char* search_file)
{
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  int found = 0;
  ///DIRECTORY
  for(int i=root_directory->first_block; i< root_directory->first_block+root_directory->blocks_num; i++)
  {
    fseek(fp, i*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    for(int j=0; j< 8; j++)
    {
      uint8_t* mask = (uint8_t*)&buf[pointer]; 
      pointer++;

      if((*mask&DIRECTORY_ENTRY_USED)==DIRECTORY_ENTRY_USED)
      {
        //skip first block info
        uint32_t* first_block = (uint32_t*)&buf[pointer];
        pointer+=4;
        uint32_t* blocks_num = (uint32_t*)&buf[pointer];
        pointer+=4;
        uint32_t* file_size = (uint32_t*)&buf[pointer];
        pointer+=18;
        //skip creation date
        //read file name
        char file_name [31];
        for(int k=0; k<31; k++)
          file_name[k] =(char)*((uint8_t*)&buf[pointer++]);
        pointer+=6;

        file->size = *file_size;
        file->size = ntohl(*file_size);
        file->first_block = ntohl(*first_block);
        file->blocks_num = ntohl(*blocks_num);
        if(strcmp(file_name, search_file)==0)
        {
          found = 1;
          printf("Copying file %s...\n", file_name);
          copy_file(fp, search_file, file);
        }
      }
    }
  }
  if(!found)
    printf("File not found\n");
};

int main ( int argc, char *argv[] )
{
  char *disk_image;
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));
  struct FDirectory *current_file;
  current_file = (struct FDirectory*)malloc(sizeof(current_file));

  if(argc != 3)

  {
    printf("Input Error. Usage: ./diskget <disk_image_file.img> <file.txt>\n");
    exit(-1);
  }

  disk_image = argv[1];
  FILE *fp;
  char *search_file = argv[2];

  fp = fopen(disk_image, "rb");
  root_directory->first_block = get_rootdir_start(fp);
  root_directory->blocks_num = get_rootdir_blocks(fp);
  //read_FAT(fp);  
  read_root_directory(fp, root_directory, current_file, search_file);

  fclose(fp);
};
