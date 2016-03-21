#include <stdio.h>
#include "fat.h"

int main ( int argc, char *argv[] )
{
  printf("Hello, world!");
  //read superblock
  FILE *fp;

  fp = fopen("img1/disk1_empty.img", "r");


  fclose(fp);
}
