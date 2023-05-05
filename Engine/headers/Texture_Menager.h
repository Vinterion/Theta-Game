#ifndef __TEXTURE_MENAGER_H__
#define __TEXTURE_MENAGER_H__

#include<SFML/Graphics.hpp>
#include<map>
#include<memory>

namespace theta {
  enum class  Texture_Id { Grass,Dirt,Rock,Sand,Snow,Wood,Leaves ,Player };

	using Textures = std::map<Texture_Id, std::shared_ptr<sf::Texture>>;
	using Texture_ptr = std::shared_ptr<sf::Texture>;

        namespace {
	  constexpr std::string_view list_of_Textures[] = { "texture.png","dirt.png","rock.png","sand.png","snow.png","wood.png","leaves.png","player.png" };
	}
	

	class Texture_Menager {
	private:
		Textures __map{};
	        std::string_view _dir_path;
	public:
	  Texture_Menager(std::string_view dir_path):_dir_path{dir_path}{};
	  [[nodiscard]]const Texture_ptr get_Texture(const Texture_Id id) ;
	};

}

#endif // !__TEXTURE_MENAGER_H__
