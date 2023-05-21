#include"headers/Physical_Object.h"

namespace theta{
  
  void Physic_Engine::collision_test() {
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
	  
  void Physic_Engine::falling_test() {
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
	_object.isfalling = search == nullptr ? true : !search->have_collision();
      }
    }
  }
  
  void Physic_Engine::item_av_test(){
    auto get_point = _object.object->get_collision_points(theta::Side_of_Collision::Bottom);
    sf::Vector2f points[3] = { get_point+sf::Vector2f{0.f,-1.f},get_point + sf::Vector2f{14.f,-1.f},get_point + sf::Vector2f{-14.f,-1.f} };
    for (const auto& point : points) {
      auto search = _chunk_menager->search(point,search_type::Item);
      if (search != nullptr) {
	_object.object->insert_item(search->type());
      }
    }
  }
}
