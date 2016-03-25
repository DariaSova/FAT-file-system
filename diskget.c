#include "headers.h"
#include "constants.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"

void read_root_directory(FILE *fp, struct FDirectory* root_directory, char* search_file)
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
        pointer+=8;
        uint32_t* file_size = (uint32_t*)&buf[pointer];
        pointer+=18;
        //skip creation date
        //read file name
        char file_name [31];
        for(int k=0; k<31; k++)
          file_name[k] =(char)*((uint8_t*)&buf[pointer++]);
        pointer+=6;

        //file_size = ntohl(&file_size);
        if(strcmp(file_name, search_file)==0)
        {
          printf("%30s\n", file_name);
        }
      }
    }
  }
};

int main ( int argc, char *argv[] )
{
  char *disk_image;
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));

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
  read_root_directory(fp, root_directory, search_file);

  fclose(fp);
};
