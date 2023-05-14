
#include<SFML/Graphics.hpp>
#include<chrono>
#include<iostream>
#include<string_view>
#include"Engine/headers/Block.h"
#include"Engine/headers/Chunk.h"
#include"Engine/headers/RTG.h"
#include"Engine/headers/Player.h"
#include"Engine/headers/Physical_Object.h"
#include"Engine/headers/filesys.h"

constexpr uint8_t NUM_OF_COLLUMNS{ 32 };
constexpr uint8_t NUM_OF_ROWS{ 24 };
constexpr float offset_pos_x = -512.f * NUM_OF_COLLUMNS;
constexpr float offset_pos_y = -512.f * NUM_OF_ROWS;
constexpr float border_r = offset_pos_x + (NUM_OF_COLLUMNS-2)*1024.f; 
constexpr float border_l = offset_pos_x + 2*1024.f;

constexpr std::string_view file_path = __FILE__ ;
constexpr std::string_view _dir_path = file_path.substr(0,file_path.size()-8LLU);


int main()
{
    
    std::ios_base::sync_with_stdio(false);
    sf::Vector2f resolution(1920, 1080);
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Theta Project");
    window.setFramerateLimit(60);
    theta::Texture_Menager Txt_menager(_dir_path);
    sf::View camera(window.getDefaultView());
    theta::Chunk_Menager chunk({offset_pos_x,offset_pos_y},NUM_OF_COLLUMNS,NUM_OF_ROWS);
    theta::RTG::Generate_Terrain(chunk, theta::seed{ 32 }, Txt_menager);
    //theta::FileManager::save_map(std::string(_dir_path)+"test.txt",chunk.get_map());
    chunk.set_Active_Chunks(camera, resolution);
    theta::Player player(sf::Vector2f{ 0.f,0.f }, camera, Txt_menager); 
    theta::Physic_Engine Physical_engine(chunk, theta::Physical_Object(player));
    while (window.isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
        sf::Event event;
	theta::Side_of_Collision side_buffor[3] = {theta::Side_of_Collision::None,theta::Side_of_Collision::None,theta::Side_of_Collision::None};
	uint8_t i_sb{0};
	while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                auto mouse_pos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                if (player.is_Range(mouse_pos)) {
		  if (event.mouseButton.button == sf::Mouse::Button::Left) chunk.destroy_block_at(mouse_pos,true);
                    else {
                        mouse_pos.x = floorf(mouse_pos.x) - static_cast<int>(mouse_pos.x) % 32;
                        mouse_pos.y = floorf(mouse_pos.y) - static_cast<int>(mouse_pos.y) % 32;
			auto tmp = player.get_item(false);
			if(tmp != theta::block_type::None)
			  if(chunk.insert(std::make_shared<theta::Block>(mouse_pos, Txt_menager,tmp)))
			    player.get_item();
			//theta::Tree::generate(mouse_pos,Txt_menager,chunk);
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed) {
	      if (event.key.code == sf::Keyboard::A
		  && player.get_collision_points(theta::Side_of_Collision::Left).x > border_l
		  && i_sb < 3)
		{ side_buffor[i_sb] = theta::Side_of_Collision::Left; ++i_sb;}
	      else if (event.key.code == sf::Keyboard::D
		       && player.get_collision_points(theta::Side_of_Collision::Right).x < border_r 
		       && i_sb < 3)
		{ side_buffor[i_sb] = theta::Side_of_Collision::Right; ++i_sb;}
	      else if (event.key.code == sf::Keyboard::W
		       && i_sb < 3)
		{ side_buffor[i_sb] = theta::Side_of_Collision::Top; ++i_sb;}
	      else if(event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5)
		player.choose_inv_slot(static_cast<size_t>(event.key.code - sf::Keyboard::Num1));
	      else if(event.key.code == sf::Keyboard::L){
		theta::FileManager::load_map(std::string(_dir_path)+"test.txt",chunk,Txt_menager);
	      }
	    }
	    
  
        }

	Physical_engine.object_moved(std::span{side_buffor});
	Physical_engine.run();
        window.clear();
        window.setView(camera);
        chunk.set_Active_Chunks(camera, resolution);
        for(const auto& x : chunk.get_Active_Chunks()) {
	  if(x == nullptr) throw std::runtime_error("Entering to not created chunk");
	  for(const auto& it : x->Draw_Chunk())
	  window.draw(*it);
        }
	player.update_inv_slot(Txt_menager);
	window.draw(player);
        window.display();
        auto end = std::chrono::high_resolution_clock::now();
        [[maybe_unused]]double time_taken = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        // std::cout << time_taken * 1e-9 << '\n';
        // system("clear");
        
    }

    return 0;
}
