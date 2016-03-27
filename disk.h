void get_name(FILE *fp, unsigned char *name)
{
  fread(name, 8, 1, fp);
}

uint32_t get_block_size(FILE *fp)
{
  uint16_t size;
  fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
  fread(&size, 2, 1, fp);
  return htons(size);
};

uint32_t get_blocks_num(FILE *fp)
{
  uint32_t blocks_num;
  fseek(fp, BLOCKCOUNT_OFFSET, SEEK_SET);
  fread(&blocks_num, 4, 1, fp);
  return ntohl(blocks_num);
};

uint32_t get_FAT_start(FILE *fp)
{
  uint32_t fat_starts;
  fseek(fp, FATSTART_OFFSET, SEEK_SET);
  fread(&fat_starts, 4, 1, fp);
  return ntohl(fat_starts);
};

uint32_t get_FAT_blocks(FILE *fp)
{
  uint32_t fat;
  fseek(fp, FATBLOCKS_OFFSET, SEEK_SET);
  fread(&fat, 4, 1, fp);
  return ntohl(fat);
};

uint32_t get_rootdir_start(FILE *fp)
{
  uint32_t dir_start;
  fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
  fread(&dir_start, 4, 1, fp);
  return ntohl(dir_start);
};

uint32_t get_rootdir_blocks(FILE *fp)
{
  uint32_t root_dir;
  fseek(fp, ROOTDIRBLOCKS_OFFSET, SEEK_SET);
  fread(&root_dir, 4, 1, fp);
  return ntohl(root_dir);
};

void read_FAT(FILE *fp)
{
  unsigned char buf[DEFAULT_BLOCK_SIZE];
  FAT->entries = (int*)malloc(sizeof(FAT->entries)*FAT->blocks_num*FAT_ENTRY_PER_BLOCK);

  FAT->free_blocks = 0;
  FAT->reserved_blocks = 0;
  FAT->allocated_blocks = 0;

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

      switch(result)
      {
        case FAT_FREE:
          FAT->free_blocks++;
          break;
        case FAT_RESERVED:
          FAT->reserved_blocks++;
          break;
        default:
          FAT->allocated_blocks++;
        break;
      }
      FAT->entries[i*FAT_ENTRY_PER_BLOCK+j] = result;
      pointer+=FAT_ENTRY_SIZE;
    }
  }
};
