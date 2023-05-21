
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include"Block.h"
#include<memory>
#include<list>
#include<unordered_map>

namespace theta {

  using Block_Vector = std::shared_ptr<std::list<std::shared_ptr<Block>>>;
  using Chunk_Search_Return = std::shared_ptr<Block>;
  using Draw_Chunk_Result = std::list<std::shared_ptr<theta::Block>>&;
  using Chunk_map = std::vector<std::list<std::shared_ptr<theta::Block>>>;
  enum class search_type{Default,Item};
  
  struct Border {
    sf::Vector2f start{};
    size_t size{};
  };
  
  constexpr bool operator<(const Border& first, const Border& second) {
    return first.start.x == second.start.x ? first.start.y < second.start.y : first.start.x < second.start.x;
	}
  constexpr bool operator==(const Border& first, const Border& second) {
    return first.start.x == second.start.x 
      && first.start.y == second.start.y 
      && first.size == second.size;
  }
  
  class Chunk {
  private:
    
    std::shared_ptr<Block> _block{nullptr};

    std::unique_ptr<Chunk> topLeft{ nullptr };
    std::unique_ptr<Chunk> topRight{ nullptr };
    std::unique_ptr<Chunk> botRight{ nullptr };
    std::unique_ptr<Chunk> botLeft{ nullptr };
		
    Block_Vector __allblocks{nullptr};
  public:
    Border border;
    //default constructor
    Chunk(const sf::Vector2f& pos = {0.f,0.f}) {
      border = { pos, 1024 };
      __allblocks = std::make_shared<std::list<std::shared_ptr<Block>>>();
    }
    //constructor for subchunks 
    Chunk(const sf::Vector2f& pos,const uint16_t size,const Block_Vector& ref) {
      border = { pos,size };
      __allblocks = ref;
    }
    //function to instert block in chunk
    bool insert(const std::shared_ptr<Block>& block);
    //function to find block at given position, return smart pointer to block 
    [[nodiscard]]const Chunk_Search_Return search(const sf::Vector2f& pos,const search_type type = search_type::Default);
    //function to destroying block at given position
    void destroy_block_at(const sf::Vector2f& pos,const bool gen_item = false);
    //return all blocks in chunk, used for drawing blocks
    [[nodiscard]] Draw_Chunk_Result Draw_Chunk() const {
      return *__allblocks.get();
    }
    //just reset chunk
    void reset(){
      __allblocks->clear();
      topLeft.reset(nullptr);
      botLeft.reset(nullptr);
      topRight.reset(nullptr);
      botRight.reset(nullptr);
    }
    
  };
  
  class Chunk_Menager {
  private:
    std::unordered_map<uint16_t, std::shared_ptr<Chunk>> _chunk_map;
    std::list<std::shared_ptr<Chunk>> list;
    sf::Vector2f _start;
    uint8_t _number_of_columns, _number_of_rows;
  public:
    //default constructor
    Chunk_Menager(const sf::Vector2f& start, const uint8_t number_of_columns, const uint8_t number_of_rows);

    //Chunk menager is unmovable and uncopable
    Chunk_Menager(const Chunk_Menager &) = delete;
    Chunk_Menager &operator=(const Chunk_Menager &) = delete;

    
    //return start position of world
    [[nodiscard]]const sf::Vector2f& get_start_point() { return _start; }
    //return size of world
    [[nodiscard]]const std::pair<uint8_t, uint8_t> get_size() { return { _number_of_columns,_number_of_rows }; }

    bool insert(std::shared_ptr<Block> block);
    void destroy_block_at(const sf::Vector2f& pos,const bool gen_item = false);
    [[nodiscard]] const Chunk_Search_Return search(const sf::Vector2f& pos,const search_type type = search_type::Default);

    //get active chunks to draw;
    [[nodiscard]]const std::list<std::shared_ptr<Chunk>>& get_Active_Chunks() { return list; }
    //set active chunks in players view 
    void set_Active_Chunks(const sf::View& camera,const sf::Vector2f& res);
    //get all chunks, used for saving the map
    [[nodiscard]]Chunk_map get_map() const{
      Chunk_map tmp;
      for(auto& it : _chunk_map){
	auto tmp2 = it.second->Draw_Chunk();
	tmp.push_back(tmp2);
      }
      return tmp;
    }
    void reset(){
      for(auto& chunk : _chunk_map) chunk.second->reset();
      list.clear();
    }
  };
  
}

#endif // !__CHUNK_H__
