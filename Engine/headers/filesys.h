#ifndef __FILESYS_H_
#define __FILESYS_H_

#include"Chunk.h"

namespace theta{
  
  struct FileManager{
    static void save_map(const std::string& path,
			 const Chunk_map& map);
    static void load_map(const std::string& path,
			 Chunk_Menager& chunk_menager,
			 Texture_Menager& txt_menager);
  }; 
}

#endif
