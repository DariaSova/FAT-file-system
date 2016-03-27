#include "headers.h"

int block(int entry)
{
  if(entry%127==0)
    return entry/127 -1;
  return entry/127;
};

int fat_entry(int entry)
{
  return entry - block(entry)*127;
};


int main ( int argc, char *argv[] )
{
  int entry = atoi(argv[1]);
  int fat_ent=fat_entry(entry);
  int fat_block=block(entry);
  printf("Entry: %d\n", entry);
  printf("Block: %d\n", fat_block);
  printf("In FAT: %d\n", fat_ent);

  fat_block++;
  if(fat_block>1)
    fat_ent--;


  printf("FAT Block: %d\n", fat_block);
  printf("FAT entry: %d\n", fat_ent);

};
