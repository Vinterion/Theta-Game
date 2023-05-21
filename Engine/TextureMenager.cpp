#include"headers/Texture_Menager.h"

namespace theta {
  namespace {
    constexpr std::string_view list_of_Textures[] = { "texture.png","dirt.png","rock.png","sand.png","snow.png","wood.png","leaves.png","player.png" };
  }
  
  const Texture_ptr Texture_Menager::get_Texture(const Texture_Id id) 
  {
    if (__map.find(id) == __map.end()) {
      sf::Texture texture;
      std::string _abs_path(_dir_path);
      _abs_path += list_of_Textures[static_cast<size_t>(id)].data();
      if(!texture.loadFromFile(_abs_path))
	throw std::runtime_error("failed to load the file");
      __map[id] = std::make_shared<sf::Texture>(std::move(texture));
    }
    return __map[id];
  }
}
