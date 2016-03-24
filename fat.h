struct Superblock {
  uint64_t system_id;
  uint16_t block_size;
  uint32_t blocks_num;
  struct FAT *FAT;
  struct FDirectory *FD;
};

struct FAT {
  uint32_t first_block;
  uint32_t blocks_num;
  uint32_t free_blocks;
  uint32_t reserved_blocks;
  uint32_t allocated_blocks;
};

struct FDirectory {
  uint32_t first_block;
  uint32_t blocks_num;
};
//define structs
struct Superblock *superblock;
struct FAT *FAT;
struct FDirectory *root_directory;
