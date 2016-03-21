#include <stdint.h>

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
};

struct FDirectory {
  uint32_t first_block;
  uint32_t blocks_num;
};
