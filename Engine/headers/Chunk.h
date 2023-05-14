
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include"Block.h"
#include<memory>
#include<list>
#include<unordered_map>

constexpr size_t MIN_SIZE_CHUNK = 32;

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
    Chunk(const sf::Vector2f& pos = {0.f,0.f}) {
      border = { pos, 1024 };
      __allblocks = std::make_shared<std::list<std::shared_ptr<Block>>>();
    }
    Chunk(const sf::Vector2f& pos,const uint16_t size,const Block_Vector& ref) {
      border = { pos,size };
      __allblocks = ref;
    }
    bool insert(const std::shared_ptr<Block>& block) {
      if (border.size == MIN_SIZE_CHUNK) {
	if (_block == nullptr) {
	  _block = block;
	  __allblocks->push_back(block);
	  return true;
	}
	else return false;
      }
      float sizeBy2 = border.size / 2.f;
      if (border.start.x + sizeBy2 > block->get_position().x) {
	if (border.start.y + sizeBy2 > block->get_position().y) {
	  if (topLeft == nullptr) topLeft = std::make_unique<Chunk>(border.start, static_cast<uint16_t>(sizeBy2) , __allblocks);
	  return topLeft->insert(block);
	}
	else {
	  if (botLeft == nullptr) botLeft = std::make_unique<Chunk>(border.start + sf::Vector2f{0.f,sizeBy2},
								    static_cast<uint16_t>(sizeBy2), __allblocks);
	  return botLeft->insert(block);
	}
      }
      else {
	if (border.start.y + sizeBy2 > block->get_position().y) {
	  if (topRight == nullptr) topRight = std::make_unique<Chunk>(border.start + sf::Vector2f{ sizeBy2,0.f },
								      static_cast<uint16_t>(sizeBy2),__allblocks);
	  return topRight->insert(block);
	}
	else {
	  if (botRight == nullptr) botRight = std::make_unique<Chunk>(border.start + sf::Vector2f{ sizeBy2,sizeBy2 },
								      static_cast<uint16_t>(sizeBy2),__allblocks);
	  return botRight->insert(block);
	}
      }
    }
    
    [[nodiscard]]const Chunk_Search_Return search(const sf::Vector2f& pos,const search_type type = search_type::Default) {
      if (_block != nullptr){
	if(type == search_type::Item && _block->is_pickable()){
	  auto tmp = _block;
	  __allblocks->remove(_block);
	  _block.reset();
	  return tmp;
	}
	else if(type == search_type::Default) return _block;
	else return nullptr;
      }
      float sizeBy2 = border.size / 2.f;
      if (border.start.x + sizeBy2 > pos.x) {
	if (border.start.y + sizeBy2 > pos.y) {
					if (topLeft == nullptr) return nullptr;
					return topLeft->search(pos,type);
	}
	else {
	  if (botLeft == nullptr) return nullptr;
	  return botLeft->search(pos,type);
	}
      }
      else {
	if (border.start.y + sizeBy2 > pos.y) {
	  if (topRight == nullptr) return nullptr;
	  return topRight->search(pos,type);
	}
	else {
	  if (botRight == nullptr) return nullptr;
	  return botRight->search(pos,type);
	}
      }
    }
    void destroy_block_at(const sf::Vector2f& pos,const bool gen_item = false) {
      if (_block != nullptr) {
	__allblocks->remove(_block);
	if(gen_item && !_block->is_pickable()){
	  Block tmp = _block->get_item();
	  _block.reset();
	  _block = std::make_unique<Block>(std::move(tmp));
	  __allblocks->push_back(_block);
	}
	else _block.reset();
	return;
      }
      float sizeBy2 = border.size / 2.f;
      if (border.start.x + sizeBy2 > pos.x) {
	if (border.start.y + sizeBy2 > pos.y) {
	  if (topLeft == nullptr) return;
	  return topLeft->destroy_block_at(pos,gen_item);
	}
	else {
	  if (botLeft == nullptr) return;
	  return botLeft->destroy_block_at(pos,gen_item);
	}
      }
      else {
	if (border.start.y + sizeBy2 > pos.y) {
	  if (topRight == nullptr) return;
	  return topRight->destroy_block_at(pos,gen_item);
	}
	else {
	  if (botRight == nullptr) return;
	  return botRight->destroy_block_at(pos,gen_item);
	}
      }
    }
    
    [[nodiscard]] Draw_Chunk_Result Draw_Chunk() const {
      return *__allblocks.get();
    }
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
    Chunk_Menager(const sf::Vector2f& start, const uint8_t number_of_columns, const uint8_t number_of_rows) :
      _start{ start }, _number_of_columns{ number_of_columns }, _number_of_rows{ number_of_rows } {
      for (uint8_t i = 0; i < number_of_columns; ++i) {
	for (uint8_t j = 0; j < number_of_rows; ++j) {
	  Chunk ctmp(start + sf::Vector2f{ 1024.f * i,1024.f * j });
	  uint16_t b_tmp = (i << 8) + j;
	  _chunk_map.emplace( b_tmp, std::make_shared<Chunk>(std::move(ctmp)) );
	}
      }
    }
    [[nodiscard]]const sf::Vector2f& get_start_point() { return _start; }
    [[nodiscard]]const std::pair<uint8_t, uint8_t> get_size() {
      return { _number_of_columns,_number_of_rows };
    };
    bool insert(std::shared_ptr<Block> block) {
      uint8_t i = static_cast<uint8_t>((block->get_position().x - _start.x) / 1024.f);
      uint8_t j = static_cast<uint8_t>((block->get_position().y - _start.y) / 1024.f);
      uint16_t b_tmp = (i << 8) + j;
      if(_chunk_map[b_tmp] != nullptr)
	return _chunk_map[b_tmp]->insert(block);
      else return false;
    }
    void destroy_block_at(const sf::Vector2f& pos,const bool gen_item = false) {
      uint8_t i = static_cast<uint8_t>((pos.x - _start.x) / 1024.f);
      uint8_t j = static_cast<uint8_t>((pos.y - _start.y) / 1024.f);
      uint16_t b_tmp = (i << 8) + j;
      return _chunk_map[b_tmp]->destroy_block_at(pos,gen_item);
    }
    [[nodiscard]] const Chunk_Search_Return search(const sf::Vector2f& pos,const search_type type = search_type::Default) {
      uint8_t i = static_cast<uint8_t>((pos.x - _start.x) / 1024.f);
      uint8_t j = static_cast<uint8_t>((pos.y - _start.y) / 1024.f);
      uint16_t b_tmp = (i << 8) + j;
      return _chunk_map[b_tmp]->search(pos,type);
    }
    [[nodiscard]]const std::list<std::shared_ptr<Chunk>>& get_Active_Chunks() {
      return list;
    }
    void set_Active_Chunks(const sf::View& camera,const sf::Vector2f& res) {
      sf::Vector2f pos{ camera.getCenter() - res*0.5f };
      list.clear();
      uint8_t i = static_cast<uint8_t>((pos.x - _start.x) / 1024.f);
      uint8_t j = static_cast<uint8_t>((pos.y - _start.y) / 1024.f);
      uint8_t imax = static_cast<uint8_t>((pos.x + res.x - _start.x) / 1024.f);
      uint8_t jmax = static_cast<uint8_t>((pos.y + res.y - _start.y) / 1024.f);
      for (; i <= imax;++i ) {
	for (auto jtmp = j; jtmp <= jmax; ++jtmp) {
	  uint16_t b_tmp = (i << 8) + jtmp;
	  list.push_back(_chunk_map[b_tmp]);
	}
      }
    }
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
