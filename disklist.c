#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "constants.h"

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
};
