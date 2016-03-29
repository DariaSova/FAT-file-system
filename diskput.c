#include "headers.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"
#include <sys/stat.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <math.h>
#include <time.h>

struct node {
  int entry;
  struct node *next;
};
struct node *root;
int dir_block=0;
int dir_entry=0;
int dir_first=0;

double file_size(int file)
{
  struct stat fileStat;
  fstat(file, &fileStat);
  return fileStat.st_size;
};

void free_entries(FILE *fp, struct FAT *FAT)
{
  struct node *current;
  current = (struct node *) malloc( sizeof(struct node) );
  current=root;
  unsigned char buf[DEFAULT_BLOCK_SIZE];
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
      struct node *next;
      next = (struct node *) malloc( sizeof(struct node) );

      switch(result)
      {
        case FAT_FREE:
          current->entry = pointer/4;
          //printf("FREE: %i  %d ", result, current->entry);
          current->next=next;
          current=next;
          FAT->free_blocks++;
          break;
      }
      pointer+=FAT_ENTRY_SIZE;
    }
  }
};

void write_file_into_dir(FILE *fp, char *file_name, int blocks_num, int file_size)
{
  
  long offset =(long)(((dir_block)*DEFAULT_BLOCK_SIZE)+(dir_entry*64));
  printf("\nOFFSET: %ld\n", offset);
  fseek(fp, offset, SEEK_SET);
  //CONVERT?
  uint8_t mask = 0x03;
  fwrite(&mask, 1, 1, fp);
  //skip first block info
  uint32_t first_block = ntohl(root->entry);
  fwrite(&first_block, 4, 1, fp);
  uint32_t blocks = ntohl(blocks_num);
  fwrite(&blocks, 4, 1, fp);
  uint32_t size = ntohl(file_size);
  fwrite(&size, 4, 1, fp);

  time_t current_time;
  current_time = time(0);
  struct tm t_now = *localtime(&current_time);
  uint16_t year = (uint16_t)(t_now.tm_year + 1900);
  year = 2016;
  year = ntohs(year);
  fwrite(&year, 2, 1, fp);

  printf("DATA: %d, %i", t_now.tm_year+1900, t_now.tm_mon);
  
  //creation time
  uint8_t month = (uint8_t)(t_now.tm_mon+1);
  fwrite(&month, 1, 1, fp);
  uint8_t day = (uint8_t)(t_now.tm_mday);
  fwrite(&day, 1, 1, fp);
  uint8_t hour = (uint8_t)(t_now.tm_hour); 
  fwrite(&hour, 1, 1, fp);
  uint8_t minute = (uint8_t)(t_now.tm_min); 
  fwrite(&minute, 1, 1, fp);
  uint8_t seconds = (uint8_t)(t_now.tm_sec);
  fwrite(&seconds, 1, 1, fp);

  //modify time
  fwrite(&year, 2, 1, fp);
  fwrite(&month, 1, 1, fp);
  fwrite(&day, 1, 1, fp);
  fwrite(&hour, 1, 1, fp);
  fwrite(&minute, 1, 1, fp);
  fwrite(&seconds, 1, 1, fp);

  int pointer = 27;

  int k=0;
  while(file_name[k]!='\0')
  {
    uint8_t c = (uint8_t)file_name[k];
    fwrite(&c, 1, 1, fp);
    pointer++;
    k++;
  }
  uint8_t c = (uint8_t)'\0';
  fwrite(&c, 1, 1, fp);
  pointer++;

  uint8_t unused = 0xFF;
  for(int i=pointer; i<64;i++)
    fwrite(&unused, 1, 1, fp);
    
};

void find_dir_block(FILE *fp)
{
  root_directory = (struct FDirectory*)malloc(sizeof(root_directory));
  root_directory->first_block = get_rootdir_start(fp);
  dir_first =root_directory->first_block;
  root_directory->blocks_num = get_rootdir_blocks(fp);
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  int found =0;

  for(int i=root_directory->first_block; i< root_directory->first_block+root_directory->blocks_num; i++)
  {
    if(found==1)
      break;

    fseek(fp, i*DEFAULT_BLOCK_SIZE, SEEK_SET);
    fread(buf, 512, 1, fp);
    int pointer = 0;

    for(int j=0; j< 8; j++)
    {
      uint8_t* mask = (uint8_t*)&buf[pointer]; 

      if((*mask&DIRECTORY_ENTRY_USED)!=DIRECTORY_ENTRY_USED)
      {
        dir_block = i;
        dir_entry =j;
        found =1;
        printf("\nDIRECTORY: %d, %d", i, j);
        break;
      }
    }
  }
};

void find_free_fat_entries(char* file)
{
  FILE *fp;
  fp = fopen(file, "rb");
  struct FAT *FAT;
  FAT = (struct FAT*)malloc(sizeof(FAT));
  FAT->first_block = get_FAT_start(fp);
  FAT->blocks_num = get_FAT_blocks(fp);
  printf("FAT starts: %d, num: %d\n", FAT->first_block, FAT->blocks_num);
  root = (struct node *) malloc( sizeof(struct node) );
  free_entries(fp, FAT);
  printf("FREEFRT: %d\n", root->entry);
  fclose(fp);
};

int blocks_needed(double size)
{
  return ceil(size/DEFAULT_BLOCK_SIZE);
}

int main ( int argc, char *argv[] )
{
  char *disk_image;

  if(argc!= 3)
  {
    printf("Input Error. Usage: ./diskput <disk_image_file.img> <file.txt>\n");
    exit(-1);
  }

  disk_image = argv[1];
  char *file_name =argv[2];
  int file=open(file_name, O_RDONLY);

  double f_size = file_size(file);
  printf("File Size: \t%f bytes\n", f_size);
  long blocks_number = blocks_needed(f_size);
  printf("File Blocks: \t%ld blocks needed\n", blocks_number);
  find_free_fat_entries(disk_image);
  FILE *fp;
  fp = fopen(disk_image, "rb");
  find_dir_block(fp);
  fclose(fp);
  fp = fopen(disk_image, "r+");
  write_file_into_dir(fp, file_name, blocks_number, f_size);

  fclose(fp);
  close(file);
};
