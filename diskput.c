#include "headers.h"
#include "constants.h"
#include "fat.h"
#include "disk.h"
#include <sys/stat.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <math.h>

struct node {
  int entry;
  struct node *next;
};
struct node *root;

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
          current->entry = pointer;
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
  int file=open(argv[2], O_RDONLY);

  double f_size = file_size(file);
  printf("File Size: \t%f bytes\n", f_size);
  long blocks_number =blocks_needed(f_size);
  printf("File Blocks: \t%ld blocks needed\n", blocks_number);
  find_free_fat_entries(disk_image);

  close(file);
};
