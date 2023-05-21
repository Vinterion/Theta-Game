#include"headers/RTG.h"
#include<random>

namespace theta{
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
    constexpr Structure_Date_Element Pine_Date_Elements[] = {{{0.f,0.f},block_type::Wood},
							     {{0.f,-32.f},block_type::Leaves},
							     {{-32.f,-32.f},block_type::Leaves},
							     {{-64.f,-32.f},block_type::Leaves},
							     {{32.f,-32.f},block_type::Leaves},
							     {{64.f,-32.f},block_type::Leaves},
							     {{0.f,-64.f},block_type::Leaves},						       
							     {{-32.f,-64.f},block_type::Leaves},
							     {{32.f,-64.f},block_type::Leaves},
							     {{0.f,-96.f},block_type::Leaves}};
  }
  
  using Tree = Structure<Tree_Date_Elements,9>;
  using Pine = Structure<Pine_Date_Elements,10>;

  enum class biomes{plain,desert,snow};

  void RTG::Generate_Terrain(Chunk_Menager& Chunk_menager, Texture_Menager& Txt_menager) {
    std::random_device r;
    std::default_random_engine rengine(r());
    
    float min_height = Chunk_menager.get_start_point().y + (Chunk_menager.get_size().second - 1) * 1024.f;
    
    std::vector<int16_t> height_date(static_cast<size_t>(Chunk_menager.get_size().first*32),0);
    
    std::vector<biomes> biomes_date(static_cast<size_t>(Chunk_menager.get_size().first*32));
		  
    //Creating biomes
    std::uniform_int_distribution<int> biome_rand(30,120);
    std::uniform_int_distribution<int> biome_type_rand(0,2);
    size_t  j = biome_rand(rengine);
    for(size_t i=0 ;i <= biomes_date.size(); i+=j){
    biomes tmp = static_cast<biomes>(biome_type_rand(rengine));
    for(auto z = i;z < j+i && z < biomes_date.size();++z){
      biomes_date[z] = tmp;
    }
    j = biome_rand(rengine);
    }
    //Creating height date
    std::uniform_int_distribution<int> num_rand_engine(40, 50);
    std::uniform_int_distribution<int> height_rand(-20,20);
    std::uniform_int_distribution<int> random_place(0,height_date.size()-1);
    auto number_of_dis = num_rand_engine(rengine);
    for (auto i1 = 0; i1 < number_of_dis; ++i1) {
      auto place_of_dis = random_place(rengine);
      auto height_dis = height_rand(rengine);
      auto offset = abs(height_dis << 2)/2;
      bool up_or_down = height_dis > 0;
      for(auto i2 = std::max(0,place_of_dis-offset);
	  i2 <= std::min(static_cast<int>(height_date.size()-1),place_of_dis+offset);++i2){
	int16_t local_height = height_dis + (abs(i2 - place_of_dis))*(up_or_down ? -1:1);
	height_date[i2] = up_or_down?
	  std::max(height_date[i2],local_height):
	  std::min(height_date[i2],local_height);
      }
    }
    //Creating Terrain
    float cur_pos = Chunk_menager.get_start_point().x;
    for(size_t i = 0; i < height_date.size();++i){
      float local_max_height = 640.f - 32.f*height_date[i];
      for (auto j = min_height; j > local_max_height; j -= 32.f) {
	int8_t block_id = 2;
	float dirt_layout_height = 32.f*(3+num_rand_engine(rengine)%4);
	if(j - local_max_height == 32.f){
	  if(biomes_date[i] == biomes::plain){
	    block_id = 0;
	    if(num_rand_engine(rengine) == 41) Tree::generate(sf::Vector2f{cur_pos,j-32.f},
							      Txt_menager,
							      Chunk_menager);
	  }
	  else if(biomes_date[i] == biomes::snow){
	    block_id = 4;
	    if(num_rand_engine(rengine) == 41) Pine::generate(sf::Vector2f{cur_pos,j-32.f},
							      Txt_menager,
							      Chunk_menager);
	  }
	  else block_id = 3;
	}
	else if(j - local_max_height < dirt_layout_height){
	  if(biomes_date[i] == biomes::plain) block_id = 1;
	  else if(biomes_date[i] == biomes::desert) block_id = 3;
	  else if(biomes_date[i] == biomes::snow) block_id = 4;
	}
	
	Chunk_menager.insert(std::make_shared<theta::Block>
			     (sf::Vector2f{cur_pos,j },
			      Txt_menager,
			      static_cast<block_type>(block_id)));
	
	
      }
      cur_pos +=32.f;
      
    }
  }
  
  
}
