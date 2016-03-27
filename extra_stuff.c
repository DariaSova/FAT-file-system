
  /*//read full block
   for(int i=0; i<63; i++)
   {
     uint64_t* current_byte = (uint64_t*)&buf[pointer];
     fwrite(current_byte, 1, 8, new_file);
     pointer+=8;
   }
   //plus 4
   uint32_t* current_byte = (uint32_t*)&buf[pointer];
   fwrite(current_byte, 1, 4, new_file);
   pointer+=4;*/


 //go through all the blocks
 /*for(int i=0; i<file->blocks_num; i++)
   {
 //read the block
 fseek(fp, file->first_block*DEFAULT_BLOCK_SIZE, SEEK_SET);
 fread(buf, 512, 1, fp);
 int pointer = 0;
 for(int j=0; j<file->size; j++)
 {
 uint8_t* current_byte = (uint8_t*)&buf[pointer]; 
 pointer++;
 fwrite(current_byte, 1, 1, new_file);
 }
 }*/
