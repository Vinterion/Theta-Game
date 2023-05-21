#ifndef __PHYSICAL_OBJECT_H__
#define __PHYSICAL_OBJECT_H__

#include<SFML/Graphics.hpp>
#include"Chunk.h"
#include"Player.h"
#include<span>

namespace theta {

  template<class  T>
  struct Physical_Object {
    T* object;
    Side_of_Collision side_of_collision{theta::Side_of_Collision::Bottom};
    bool isfalling{true};
    float y_velocity{ 0.f };
    float x_velocity{ 0.f };
    Physical_Object(T& player) { object = &player; };
  };
  
  constexpr bool is_intersect32f(const sf::Vector2f& first, const sf::Vector2f& second) {
    return second.x < first.x
	   || second.x + 32.f > first.x
           || second.y < first.y
           || second.y + 32.f > first.y;
    
  }

  [[nodiscard]] inline const sf::Vector2f collision_overlap32f(const sf::Vector2f& first, const sf::Vector2f& second,const Side_of_Collision& side) { 
    switch (side)
      {
      case Side_of_Collision::Top:
	return sf::Vector2f(0.f, -first.y + (second.y + 32.f));
	break;
      case Side_of_Collision::Bottom:
	return sf::Vector2f(0.f, -first.y + (second.y));
	break;
      case Side_of_Collision::Left:
	return sf::Vector2f(((second.x + 32.f) - first.x), 0.f);
	break;
      case Side_of_Collision::Right:
	return sf::Vector2f(((second.x) - first.x), 0.f);
	break;
      default:
	return sf::Vector2f(0.f,0.f);
	break;
      }
  }

  class Physic_Engine {
  private:
    Chunk_Menager* _chunk_menager;
    Physical_Object<Player> _object;
    
    //Testing collision with blocks and items
    void collision_test();
    //Simulating gravity
    void falling_test();
    //Picking items from map
    void item_av_test();
    
  public:
    Physic_Engine(Chunk_Menager& chunk_menager, Physical_Object<Player>&& object) : _object{ object } {
      _chunk_menager = &chunk_menager;
    }
    
    //Physic_Engine is unmovable and uncopable
    Physic_Engine(const Physic_Engine &) = delete;
    Physic_Engine &operator=(const Physic_Engine &) = delete;
    
    void run(){
      _object.object->move({ _object.x_velocity,-_object.y_velocity });
      item_av_test();
      falling_test();
      collision_test();
    }
    void object_moved(const std::span<theta::Side_of_Collision> side_buffor) {
      for(auto& side : side_buffor){
	if (side == theta::Side_of_Collision::Top && _object.y_velocity == 0.f && !_object.isfalling) {
	  _object.y_velocity += 40.f;
	}
	else if (side == theta::Side_of_Collision::Right && _object.x_velocity < 24.f) {
	  _object.side_of_collision = side;
	  _object.x_velocity += 12.f;
	}
	else if (side == theta::Side_of_Collision::Left && _object.x_velocity > -24.f) {
	  _object.side_of_collision = side;
	  _object.x_velocity -= 12.f;
	}
      }
    }
  };
}

#endif // !__PHYSICAL_OBJECT_H__
