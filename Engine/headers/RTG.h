#ifndef __RTG_H__
#define __RTG_H__

#include"Chunk.h"

namespace theta {
  struct Structure_Date_Element{
    float offset[2];
    block_type Type;
  };
  template<const Structure_Date_Element* Date,size_t N>
  struct Structure{
    static void generate(const sf::Vector2f& start_position,
		         Texture_Menager& Txt_menager,
			 Chunk_Menager& Chunk_menager){
      for(size_t i = 0; i < N ; ++i)
	Chunk_menager.insert(std::make_shared<Block>(
						     start_position + sf::Vector2f{Date[i].offset[0],Date[i].offset[1]},
						     Txt_menager,
						     Date[i].Type));
    }
    
  };
  
  struct RTG {
    static void Generate_Terrain(Chunk_Menager& Chunk_menager, Texture_Menager& Txt_menager);
  };

}

#endif // !__RTG_H__
