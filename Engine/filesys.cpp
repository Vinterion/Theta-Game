#include"headers/filesys.h"
#include<sstream>
#include<fstream>
namespace theta{
  void FileManager::save_map(const std::string& path,
			     const Chunk_map& map){
    std::ofstream file(path);
    for(auto& chunk : map){
      for(const auto& block : chunk){
	auto x = block->get_position().x;
	auto y =  block->get_position().y;
	int16_t x_i = static_cast<int16_t>(x*(1.f/32.f));
	int16_t y_i = static_cast<int16_t>(y*(1.f/32.f));
	int32_t i = (x_i << 16) + y_i;
	file << i << ',' << static_cast<int>(block->type()) << '\n';
      }
    }
  }
  void FileManager::load_map(const std::string& path,
			     Chunk_Menager& chunk_menager,
			     Texture_Menager& txt_menager){
    std::ifstream file(path);
    std::string buffor{};
    if(file.is_open()){
      chunk_menager.reset();
      while (std::getline(file, buffor)) {
	std::stringstream line(buffor);
	std::string tmp{};
	std::getline(line, tmp, ',');
	int32_t i = stol(tmp);
	std::getline(line, tmp);
	auto type = static_cast<block_type>(stoi(tmp));
	auto x = (i>>16)*32.f;
	auto y = (i & static_cast<long>((1<<16)-1))*32.f;
	chunk_menager.insert(std::make_shared<Block>(sf::Vector2f{x,y},txt_menager,type));
      }
    }
    else throw std::runtime_error("Cannot open save file");
  }
}
