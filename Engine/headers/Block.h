#ifndef __BLOCK_H__
#define __BLOCK_H__

#include<SFML/Graphics.hpp>
#include"Texture_Menager.h"

namespace theta {
  enum class block_type {None=-1,Grass,Dirt,Rock,Sand,Snow,Wood,Leaves};
  class Block : public sf::Drawable {
  private:
    sf::Vertex __vectors[4]{};
    Texture_ptr texture{nullptr};
    //Properties
    block_type _type{block_type::None};
    bool _Have_Collision{true};
    bool _Have_Gravity{false};
    bool _Pickable{false};
    virtual void draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
    {
      states.blendMode = sf::BlendNone;
      states.texture = texture.get();
      target.draw(__vectors,4U, sf::Quads, states);
    }
    Block(sf::Vector2f position,Texture_ptr txt_ptr,const block_type Type) {
      //setting texture for block
      texture = txt_ptr;
      //setting vertecies
      position += sf::Vector2f{8.f,0.f};
      
      __vectors[0].position = position;
      
      __vectors[1].position = position + sf::Vector2f{16.f,0.f};
      
      __vectors[1].texCoords = sf::Vector2f{ 32.f,0.f };
      
      __vectors[2].position = position + sf::Vector2f{ 16.f,-16.f };
      
      __vectors[2].texCoords = sf::Vector2f{ 32.f,32.f };
      
      __vectors[3].position = position + sf::Vector2f{ 0.f,-16.f };
      
      __vectors[3].texCoords = sf::Vector2f{ 0.f,32.f };

      //Setting Block properties
      _Have_Collision = false;
      _type = Type;
      _Pickable = true;
    }
  public:
    Block(const sf::Vector2f& position,Texture_Menager& Txt_menager,const block_type Type) {
      //setting texture for block
      texture = Txt_menager.get_Texture(static_cast<Texture_Id>(Type));
      //setting vertecies 
      __vectors[0].position = position;
      
      __vectors[1].position = position + sf::Vector2f{32.f,0.f};
      
      __vectors[1].texCoords = sf::Vector2f{ 32.f,0.f };
      
      __vectors[2].position = position + sf::Vector2f{ 32.f,32.f };
      
      __vectors[2].texCoords = sf::Vector2f{ 32.f,32.f };
      
      __vectors[3].position = position + sf::Vector2f{ 0.f,32.f };
      
      __vectors[3].texCoords = sf::Vector2f{ 0.f,32.f };

      //Setting Block properties
      _Have_Collision = !(Type == block_type::Wood || Type == block_type::Leaves);
      _type = Type;
    }
    [[nodiscard]]const sf::Vector2f& get_position() const {
      return __vectors[0].position;
    }
    [[nodiscard]]bool have_collision() const {
      return _Have_Collision;
    }
    [[nodiscard]]block_type type() const {
      return _type;
    }
    [[nodiscard]] Block get_item() {
      return Block(__vectors[3].position,texture,_type);
    }
    [[nodiscard]] bool is_pickable() const {
      return _Pickable;
    }
  };

}

#endif // !__BLOCK_H__
