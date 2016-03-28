#include "headers.h"
#include "constants.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"

int fat_block(int entry)
{
  if(entry<=127)
    return 0;
  else
    if((entry-127)%128==0)
      return (entry-127)/128;
    else
      return (entry-127)/128+1;
};

int fat_entry(int entry)
{
  int result;
  if(entry<=127)
    return entry;
  else
    result = entry-127 - ((entry-127)/128)*128;
  if(result>0)
    return result-1;
  else
    return 127;
};

void copy_file(FILE *fp, char* file_name, struct FDirectory* file)
{
  printf("File name: %s, First: %d, Size: %i, Blocks num: %i\n", file_name, file->first_block, file->size, file->blocks_num);
  FILE* new_file = fopen(file_name, "wb");
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  unsigned char fat_buf[DEFAULT_BLOCK_SIZE];

  int full_blocks_num = file->size/DEFAULT_BLOCK_SIZE;
  int rem_bytes = file->size%DEFAULT_BLOCK_SIZE;
  uint32_t current_block;
  current_block = file->first_block;
  uint8_t* current_byte; 

  //read FAT
  unsigned char buf2[DEFAULT_BLOCK_SIZE*120];
  fseek(fp, DEFAULT_BLOCK_SIZE, SEEK_SET);
  fread(buf2, DEFAULT_BLOCK_SIZE, 120, fp);

  for(int i=0; i<full_blocks_num; i++)
  {

    fseek(fp, (long)((current_block)*DEFAULT_BLOCK_SIZE), SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    //uint32_t* next_block = (uint32_t*)&buf[pointer];
    //pointer+=4;
    //fwrite(next_block, 1, 4, new_file);

    for(int j=0; j<512; j++)
    {
      current_byte = (uint8_t*)&buf[pointer]; 
      pointer++;
      fwrite(current_byte, 1, 1, new_file);
    }

    if(i==(file->blocks_num-1))
    {
      printf("\nCOUNTER: %d\n", i);
      printf("NEXT BLOCK: %d POinter: %d\n", current_block, pointer);
      printf("\nDONE!");
    }
    else 
    { 

      uint32_t full_fat = fat_block(current_block);
      uint32_t rem_fat = fat_entry(current_block);
      //printf("\n full_fat: %d, remaining: %d\n", full_fat, rem_fat); 
      full_fat++;
      //rem_fat--;
      //if(full_fat>1)
      //  rem_fat-=2;
      if(current_block==0)
      {
        full_fat =1;
        rem_fat=0;
      }



      printf("\n full_fat: %d, remaining: %d\n", full_fat, rem_fat); 
      int ptr = (long)(rem_fat); 
      fseek(fp, (long)full_fat*DEFAULT_BLOCK_SIZE, SEEK_SET);
      fread(fat_buf, 512, 1, fp);
      uint32_t* next = (uint32_t*)&fat_buf[ptr*4];
      current_block = ntohl(*next);
      printf("NEXT BLOCK: %d POinter: %d\n", current_block, pointer);


/*
      ////works here
      uint32_t* next2 = (uint32_t*)&buf2[current_block*4];
      current_block = ntohl(*next2);
      printf("NEXT BLOCK: %d POinter: %d\n", current_block, pointer);
*/
    }
  }

  //copy remaining bytes
  if(rem_bytes>0)
  {
    printf("\nOOOOps");
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
          printf("%30s\n", file_name);
          copy_file(fp, search_file, file);
        }
        else
        {
          //printf("File not found");
        }
      }
    }
  }
};

int main ( int argc, char *argv[] )
{
  char *disk_image;
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));
  struct FDirectory *current_file;
  current_file = (struct FDirectory*)malloc(sizeof(current_file));
  //FAT = (struct FAT*)malloc(sizeof(FAT));

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
