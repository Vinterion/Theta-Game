#ifndef __PLAYER_H__
#define __PLAYER_H__

#include"Texture_Menager.h"
#include"Block.h"
#include<array>

namespace theta {

  enum class Side_of_Collision { Top, Bottom, Left, Right,None };
  using Inventory =  std::array<std::pair<block_type,uint8_t>,5>;
  
  class Player : public sf::Drawable {
  private: 
    sf::Vertex __vectors[4]{};
    sf::Vector2f __collision_points[4]{};
    sf::View* _camera;
    Texture_ptr texture{ nullptr };
    Texture_ptr inv_txt_slot{nullptr};
    Inventory inv;
    size_t inv_pos{0};
    sf::RectangleShape in_side_inv,out_side_inv;
    sf::Text slot_id,num_items;
    virtual void draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
    {
      states.texture = texture.get();
      target.draw(out_side_inv);
      target.draw(in_side_inv);
      target.draw(slot_id);
      target.draw(num_items);
      target.draw(__vectors, 4U, sf::Quads, states);
    }
  public:
    Player(const sf::Vector2f& start_pos,sf::View& camera,Texture_Menager& Txt_menager){
      //getting Texture for model
      texture = Txt_menager.get_Texture(theta::Texture_Id::Dirt);
      auto size = texture->getSize();
	    //setting verticies
      __vectors[0].position = start_pos;
      __vectors[1].position = start_pos + sf::Vector2f{ size.x*1.f,0.f };
      __vectors[1].texCoords = sf::Vector2f{ size.x * 1.f,0.f };
      __vectors[2].position = start_pos + sf::Vector2f{ size.x * 1.f,size.y*1.f };
      __vectors[2].texCoords = sf::Vector2f{ size.x * 1.f,size.y * 1.f };
      __vectors[3].position = start_pos + sf::Vector2f{ 0.f,size.y*1.f };
      __vectors[3].texCoords = sf::Vector2f{ 0.f,size.y * 1.f };
      //binding camera
      _camera = &camera;
      //making collision points
      __collision_points[0] = sf::Vector2f((__vectors[0].position.x + __vectors[1].position.x) / 2.f, __vectors[0].position.y);
      __collision_points[1] = sf::Vector2f((__vectors[0].position.x + __vectors[1].position.x) / 2.f, __vectors[2].position.y);
      __collision_points[2] = sf::Vector2f(__vectors[0].position.x, (__vectors[2].position.y + __vectors[0].position.y) / 2.f);
      __collision_points[3] = sf::Vector2f(__vectors[1].position.x, (__vectors[2].position.y + __vectors[0].position.y) / 2.f);
      //Filling inventory with nothing
      inv.fill(std::make_pair(block_type::None,static_cast<uint8_t>(0)));
      //Setting inventory gui
      out_side_inv.setSize(sf::Vector2f{48.f,48.f});
      out_side_inv.setPosition(camera.getCenter()+sf::Vector2f{camera.getSize().x/2.f-48.f,-camera.getSize().y/2.f});
      out_side_inv.setFillColor(sf::Color(0x44,0x44,0x44));

      in_side_inv.setSize(sf::Vector2f{32.f,32.f});
      in_side_inv.setPosition(camera.getCenter()+sf::Vector2f{camera.getSize().x/2.f-40.f,-camera.getSize().y/2.f+8.f});
      in_side_inv.setFillColor(sf::Color::Transparent);
      in_side_inv.setTexture(inv_txt_slot.get());

      slot_id.setFont(Txt_menager.get_font());
      slot_id.setCharacterSize(8);
      slot_id.setFillColor(sf::Color::White);
      slot_id.setPosition(out_side_inv.getPosition() + sf::Vector2f{2.f,2.f});
      slot_id.setString("1");

      num_items.setFont(Txt_menager.get_font());
      num_items.setCharacterSize(8);
      num_items.setFillColor(sf::Color::White);
      num_items.setPosition(out_side_inv.getPosition() + sf::Vector2f{20.f,32.f});
    }
    [[nodiscard]]const sf::Vector2f get_collision_points(const theta::Side_of_Collision side) {
      return __collision_points[static_cast<size_t>(side)];
    }
    [[nodiscard]]bool is_Range(const sf::Vector2f& pos) {
      return powf((pos.x - __collision_points[0].x), 2.f) + powf((pos.y - __collision_points[2].y), 2.f) < 2304.f;
    }
    void choose_inv_slot(const size_t i){
      if(i > 4) throw std::runtime_error("inv slot number is too big");
      else[[likely]] {
	inv_pos = i;
	slot_id.setString(std::to_string(i+1));
      }
    }
    void insert_item(const block_type type){
      for(auto& it : inv){
	if(it.first == block_type::None){
	  it.first = type;
	  it.second = 1;
	  break;
	}
	else if(it.first == type && it.second < 0xFF){
	  ++it.second;
	  break;
	}
      }
    }
    block_type get_item(const bool remove_item = true){
      if(remove_item){
	if(inv[inv_pos].first == block_type::None)
	  return block_type::None;
	else if (inv[inv_pos].second == 1){
	  auto tmp = inv[inv_pos].first; 
	  inv[inv_pos].first = block_type::None;
	  inv[inv_pos].second = 0;
	  return tmp;
	}
	else {
	  --inv[inv_pos].second;
	  return inv[inv_pos].first;
	}
      }
      else return inv[inv_pos].first;
    }
    void update_inv_slot(Texture_Menager& txt_menager){
      static size_t previous_i{0};
      static block_type previous_type{block_type::None};
      static uint8_t previous_num{0};
      if(previous_i != inv_pos || previous_type != inv[inv_pos].first || previous_num != inv[inv_pos].second){
	previous_i = inv_pos;
	auto [type,num] = inv[inv_pos];
	previous_type = type;
	previous_num = num;
	if(type == block_type::None) {
	  inv_txt_slot = nullptr;
	  in_side_inv.setFillColor(sf::Color::Transparent);
	  num_items.setString("");
	}
	else {
	  inv_txt_slot = txt_menager.get_Texture(static_cast<Texture_Id>(type));
	  in_side_inv.setFillColor(sf::Color::White);
	  num_items.setString(std::to_string(num));
	}
      in_side_inv.setTexture(inv_txt_slot.get());
      }
    }
    // void destroy_block(const sf::Vector2f& pos,Chunk_menager& chunk){
	    
    // }
    // void insert_block(const sf::Vector2f& pos,Chunk_menager& chunk){
    
    // }
    void move(const sf::Vector2f& move_vec) {
      for (auto& vecs : __vectors) vecs.position += move_vec;
      for (auto& cpoints : __collision_points) cpoints += move_vec;
      sf::Vector2f movement = __vectors[0].position - _camera->getCenter();
      _camera->move(movement);
      out_side_inv.move(movement);
      in_side_inv.move(movement);
      slot_id.move(movement);
      num_items.move(movement);
    }
  };
  
}
#endif // !__PLAYER_H__

