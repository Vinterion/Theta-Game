#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include "Chunk.h"

namespace theta{

  struct Structure_Date_Element{
    float offset[2];
    block_type Type;
  };
  template<const Structure_Date_Element* Date,size_t N>
  struct Structure{
    static void generate(const sf::Vector2f& start_position,
		    Texture_Menager& Txt_menager,Chunk_Menager& Chunk_menager){
      for(size_t i = 0; i < N ; ++i)
	Chunk_menager.insert(std::make_shared<Block>(
						     start_position + sf::Vector2f{Date[i].offset[0],Date[i].offset[1]},
						     Txt_menager,
						     Date[i].Type));
    }

  };
  namespace{
    constexpr Structure_Date_Element Tree_Date_Elements[] = {{{0.f,0.f},block_type::Wood},
							    {{0.f,-32.f},block_type::Wood},
							    {{0.f,-64.f},block_type::Wood},
							    {{0.f,-96.f},block_type::Leaves},
							    {{-32.f,-96.f},block_type::Leaves},
							    {{32.f,-96.f},block_type::Leaves},
							    {{0.f,-128.f},block_type::Leaves},
							    {{-32.f,-128.f},block_type::Leaves},
							    {{32.f,-128.f},block_type::Leaves}
    };
  }
  using Tree = Structure<Tree_Date_Elements,9>;
};


#endif
