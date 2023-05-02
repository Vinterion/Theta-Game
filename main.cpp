#include<SFML/Graphics.hpp>
#include<chrono>
#include<iostream>
#include<string_view>
#include"Engine/headers/Block.h"
#include"Engine/headers/Chunk.h"
#include"Engine/headers/RTG.h"
#include"Engine/headers/Player.h"
#include"Engine/headers/Physical_Object.h"

static constexpr uint8_t NUM_OF_COLLUMNS{ 24 };
static constexpr uint8_t NUM_OF_ROWS{ 24 };
static constexpr float offset_pos_x = -512.f * NUM_OF_COLLUMNS;
static constexpr float offset_pos_y = -512.f * NUM_OF_ROWS;
static constexpr std::string_view file_path {__FILE__};
// static constexpr std::string_view dir_path = file_path.substr(0,3);

int main()
{
    std::string dir_path{file_path};
    dir_path = dir_path.substr(0,dir_path.size()-8);
    std::ios_base::sync_with_stdio(false);
    sf::Vector2f resolution(1920, 1080);
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Theta Project");
    window.setFramerateLimit(60);
    sf::View camera(window.getDefaultView());
    theta::Texture_Menager Txt_menager(dir_path);
    theta::Chunk_Menager chunk({offset_pos_x,offset_pos_y},NUM_OF_COLLUMNS,NUM_OF_ROWS);
    theta::RTG::Generate_Terrain(chunk, theta::seed{ 32 }, Txt_menager);
    chunk.set_Active_Chunks(camera, resolution);
    theta::Player player(sf::Vector2f{ 0.f,0.f }, camera, Txt_menager);
    theta::Physic_Engine Physical_engine(chunk, theta::Physical_Object(player));
    while (window.isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
        sf::Event event;
	theta::Side_of_Collision side_buffor[] = {theta::Side_of_Collision::None,theta::Side_of_Collision::None,theta::Side_of_Collision::None};
	uint8_t i_sb{0};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                auto mouse_pos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                if (player.is_Range(mouse_pos)) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) chunk.destroy_block_at(mouse_pos);
                    else {
                        mouse_pos.x = floorf(mouse_pos.x) - static_cast<int>(mouse_pos.x) % 32;
                        mouse_pos.y = floorf(mouse_pos.y) - static_cast<int>(mouse_pos.y) % 32;
                        chunk.insert(std::make_shared<theta::Block>(mouse_pos, Txt_menager, theta::Texture_Id::Dirt));
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed && i_sb < 3) {
	      if (event.key.code == sf::Keyboard::A) { side_buffor[i_sb] = (theta::Side_of_Collision::Left); ++i_sb;}
	      else if (event.key.code == sf::Keyboard::D){ side_buffor[i_sb] = (theta::Side_of_Collision::Right); ++i_sb;}
	      else if (event.key.code == sf::Keyboard::W){ side_buffor[i_sb] = (theta::Side_of_Collision::Top); ++i_sb;}
            }
  
        }
	Physical_engine.object_moved(std::span{side_buffor});
        window.clear();
        window.setView(camera);
        chunk.set_Active_Chunks(camera, resolution);
        for (const auto& x : chunk.get_Active_Chunks()) {
            for(const auto& it : x->Draw_Chunk())
            window.draw(*it);
        }

	window.draw(player);
        window.display();
        auto end = std::chrono::high_resolution_clock::now();
        [[maybe_unused]]double time_taken = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        // std::cout << time_taken * 1e-9 << '\n';
        // system("clear");
        
    }

    return 0;
}
