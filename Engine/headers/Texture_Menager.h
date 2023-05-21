#ifndef __TEXTURE_MENAGER_H__
#define __TEXTURE_MENAGER_H__

#include<SFML/Graphics.hpp>
#include<map>
#include<memory>

namespace theta {
  enum class  Texture_Id { Grass,Dirt,Rock,Sand,Snow,Wood,Leaves ,Player };
  
  using Textures = std::map<Texture_Id, std::shared_ptr<sf::Texture>>;
  using Texture_ptr = std::shared_ptr<sf::Texture>;  

  class Texture_Menager {
  private:
    Textures __map{};
    sf::Font font;
    std::string_view _dir_path;
  public:
    Texture_Menager(std::string_view dir_path):_dir_path{dir_path}{
      if(!font.loadFromFile(std::string(dir_path) + std::string("minecraft_font.ttf")))
	throw std::runtime_error("Cannot load font file");
    }

    //Texture menager is unmovable and uncopable
    Texture_Menager(const Texture_Menager &) = delete;
    Texture_Menager &operator=(const Texture_Menager &) = delete;

    [[nodiscard]]const Texture_ptr get_Texture(const Texture_Id id);
    [[nodiscard]]const sf::Font& get_font(){return font;}
  };
  
}

#endif // !__TEXTURE_MENAGER_H__
