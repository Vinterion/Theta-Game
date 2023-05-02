#ifndef __PLAYER_H__
#define __PLAYER_H__

#include"Texture_Menager.h"

namespace theta {

	enum class Side_of_Collision { Top, Bottom, Left, Right,None };

	class Player : public sf::Drawable {
	private:
		sf::Vertex __vectors[4]{};
		sf::Vector2f __collision_points[4]{};
		sf::View* _camera;
		Texture_ptr texture{ nullptr };
		virtual void draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
		{
			states.texture = texture.get();
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
		}
		[[nodiscard]]const sf::Vector2f get_collision_points(const theta::Side_of_Collision side) {
			return __collision_points[static_cast<size_t>(side)];
		}
		[[nodiscard]] const bool is_Range(const sf::Vector2f& pos) {
			return powf((pos.x - __collision_points[0].x), 2.f) + powf((pos.y - __collision_points[2].y), 2.f) < 2304.f;
		}
		void move(const sf::Vector2f& move_vec) {
			for (auto& vecs : __vectors) vecs.position += move_vec;
			for (auto& cpoints : __collision_points) cpoints += move_vec;
			sf::Vector2f movement = __vectors[0].position - _camera->getCenter();
			_camera->move(movement);
		}
	};

}
#endif // !__PLAYER_H__

