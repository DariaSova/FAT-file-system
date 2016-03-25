#include "headers.h"
#include "constants.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"

void copy_file(FILE *fp, char* file_name, struct FDirectory* file)
{
  printf("File name: %s, First: %d, Size: %i, Blocks num: %i\n", file_name, file->first_block, file->size, file->blocks_num);
  FILE* new_file = fopen(file_name, "wb");
  unsigned char buf[DEFAULT_BLOCK_SIZE];

  //go through all the blocks
  for(int i=0; i<file->blocks_num; i++)
  {
    //read the block
    fseek(fp, file->first_block*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;
    for(int j=0; j<7; j++)
    {
      uint8_t* current_byte = (uint8_t*)&buf[pointer]; 
      pointer+=8;
      fwrite(current_byte, 1, sizeof(current_byte), new_file);
    }
  }

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
  read_root_directory(fp, root_directory, current_file, search_file);

  fclose(fp);
};
