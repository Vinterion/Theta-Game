#include"headers/Chunk.h"

namespace theta{

  namespace {
    constexpr size_t MIN_SIZE_CHUNK = 32;
    constexpr float CHUNK_DIV_MUL = 1.f/1024.f;
  }
  
  bool Chunk::insert(const std::shared_ptr<Block>& block) {
    if (border.size == MIN_SIZE_CHUNK) {
      if (_block == nullptr) {
	_block = block;
	__allblocks->push_back(block);
	return true;
      }
      else return false;
    }
    float sizeBy2 = border.size * 0.5f;
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
  
  const Chunk_Search_Return Chunk::search(const sf::Vector2f& pos,const search_type type) {
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
    float sizeBy2 = border.size * 0.5f;
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
  
  void Chunk::destroy_block_at(const sf::Vector2f& pos,const bool gen_item) {
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
    float sizeBy2 = border.size * 0.5f;
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
  
  Chunk_Menager::Chunk_Menager(const sf::Vector2f& start, const uint8_t number_of_columns, const uint8_t number_of_rows) :
    _start{ start }, _number_of_columns{ number_of_columns }, _number_of_rows{ number_of_rows } {
    for (uint8_t i = 0; i < number_of_columns; ++i) {
      for (uint8_t j = 0; j < number_of_rows; ++j) {
	Chunk ctmp(start + sf::Vector2f{ 1024.f * i,1024.f * j });
	uint16_t b_tmp = (i << 8) + j;
	_chunk_map.emplace( b_tmp, std::make_shared<Chunk>(std::move(ctmp)) );
      }
    }
  }

  bool Chunk_Menager::insert(std::shared_ptr<Block> block) {
    uint8_t i = static_cast<uint8_t>((block->get_position().x - _start.x) * CHUNK_DIV_MUL);
    uint8_t j = static_cast<uint8_t>((block->get_position().y - _start.y) * CHUNK_DIV_MUL);
    uint16_t b_tmp = (i << 8) + j;
    if(_chunk_map[b_tmp] != nullptr)
      return _chunk_map[b_tmp]->insert(block);
    else return false;
  }

  void Chunk_Menager::destroy_block_at(const sf::Vector2f& pos,const bool gen_item) {
    uint8_t i = static_cast<uint8_t>((pos.x - _start.x) * CHUNK_DIV_MUL);
    uint8_t j = static_cast<uint8_t>((pos.y - _start.y) * CHUNK_DIV_MUL);
    uint16_t b_tmp = (i << 8) + j;
    return _chunk_map[b_tmp]->destroy_block_at(pos,gen_item);
  }

  [[nodiscard]] const Chunk_Search_Return Chunk_Menager::search(const sf::Vector2f& pos,const search_type type) {
    uint8_t i = static_cast<uint8_t>((pos.x - _start.x) * CHUNK_DIV_MUL);
    uint8_t j = static_cast<uint8_t>((pos.y - _start.y) * CHUNK_DIV_MUL);
    uint16_t b_tmp = (i << 8) + j;
    return _chunk_map[b_tmp]->search(pos,type);
  }

  void Chunk_Menager::set_Active_Chunks(const sf::View& camera,const sf::Vector2f& res) {
    sf::Vector2f pos{ camera.getCenter() - res*0.5f };
    list.clear();
    uint8_t i = static_cast<uint8_t>((pos.x - _start.x) * CHUNK_DIV_MUL);
    uint8_t j = static_cast<uint8_t>((pos.y - _start.y) * CHUNK_DIV_MUL);
    uint8_t imax = static_cast<uint8_t>((pos.x + res.x - _start.x) * CHUNK_DIV_MUL);
    uint8_t jmax = static_cast<uint8_t>((pos.y + res.y - _start.y) * CHUNK_DIV_MUL);
    for (; i <= imax;++i ) {
      for (auto jtmp = j; jtmp <= jmax; ++jtmp) {
	uint16_t b_tmp = (i << 8) + jtmp;
	list.push_back(_chunk_map[b_tmp]);
      }
    }
  }
}
