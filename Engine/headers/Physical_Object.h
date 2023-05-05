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

	inline const sf::Vector2f collision_overlap32f(const sf::Vector2f& first, const sf::Vector2f& second,const Side_of_Collision& side) {
		sf::Vector2f result{ 0.f,0.f };
		if (first != second) {
			switch (side)
			{
			case Side_of_Collision::Top:
				result = sf::Vector2f(0.f, -first.y + (second.y + 32.f));
				break;
			case Side_of_Collision::Bottom:
				result = sf::Vector2f(0.f, -first.y + (second.y));
				break;
			case Side_of_Collision::Left:
				result = sf::Vector2f(((second.x + 32.f) - first.x), 0.f);
				break;
			case Side_of_Collision::Right:
				result = sf::Vector2f(((second.x) - first.x), 0.f);
				break;
			default:
				break;
			}
		}
		return result;
	}

	class Physic_Engine {
	private:
		Chunk_Menager* _chunk_menager;
		Physical_Object<Player> _object;

		void collision_test() {
			if (_object.x_velocity != 0.f) {
				_object.x_velocity -= _object.x_velocity / 2.f;
				auto search = _chunk_menager->search(_object.object->get_collision_points(_object.side_of_collision));
					if (search != nullptr) {
					  if(search->have_collision()){
						_object.x_velocity = 0.f;
						_object.object->move(theta::collision_overlap32f(_object.object->get_collision_points(_object.side_of_collision),
												 search->get_position(), _object.side_of_collision));

					  }

					}
			}
		}

		void falling_test() {
			if (_object.isfalling) {
			  if(_object.y_velocity > -24.f) _object.y_velocity -= 12.f; 
			  auto get_point = _object.object->get_collision_points(theta::Side_of_Collision::Bottom);
			  sf::Vector2f points[3] = { get_point,get_point + sf::Vector2f{14.f,0},get_point + sf::Vector2f{-14.f,0} };
			  for (const auto& point : points) {
			     auto search = _chunk_menager->search(point);
			     if (search != nullptr) {
			       if(search->have_collision()){
				_object.object->move(theta::collision_overlap32f(point, search->get_position(), theta::Side_of_Collision::Bottom));
				_object.isfalling = false;
				_object.y_velocity = 0.f;
				break;
			       }
			     }
			  }
			}
			else {
				if (_object.y_velocity > 0.f) {
					_object.y_velocity -=10.f;
					if (_object.y_velocity == 0.f) _object.isfalling = true;
					auto get_point = _object.object->get_collision_points(theta::Side_of_Collision::Top);
					sf::Vector2f points[3] = { get_point,get_point + sf::Vector2f{14.f,0},get_point + sf::Vector2f{-14.f,0} };
					for (const auto& point : points) {
						auto search = _chunk_menager->search(point);
						if (search != nullptr) {
						  if(search->have_collision()){
						    _object.object->move(theta::collision_overlap32f(point, search->get_position(), theta::Side_of_Collision::Top));
						    _object.y_velocity = 0.f;
						    _object.isfalling = true;
						    break;
						  }
						}
					}
				}
				else {
					auto search = _chunk_menager->search(_object.object->get_collision_points(theta::Side_of_Collision::Bottom));
					_object.isfalling = search == nullptr;
				}
			}
		}

	public:
		Physic_Engine(Chunk_Menager& chunk_menager, Physical_Object<Player>&& object) : _object{ object } {
			_chunk_menager = &chunk_menager;
		}
		void run(){
		        _object.object->move({ _object.x_velocity,-_object.y_velocity });
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
