#ifndef __APP_H_
#define __APP_H_

#include<SFML/Graphics.hpp>
#include<iostream>
#include<string_view>
#include"Engine/headers/Block.h"
#include"Engine/headers/Chunk.h"
#include"Engine/headers/RTG.h"
#include"Engine/headers/Player.h"
#include"Engine/headers/Physical_Object.h"
#include"Engine/headers/filesys.h"

namespace theta{

  struct settings{
    const uint8_t NUM_OF_COLLUMNS;
    const uint8_t NUM_OF_ROWS;
    const float offset_pos_x;
    const float offset_pos_y;
    const float border_r; 
    const float border_l;
    const std::string_view _dir_path;
    float res_x;
    float res_y;    
  };
  
  class app{
  private:
    settings _sets;
    sf::RenderWindow* window;
    Texture_Menager* Txt_menager;
    void menu(){
      //init menu boxes
      sf::RectangleShape box1,box2,box3,in_box;
      sf::Text text1,text2,text3,in_text;

      box1.setSize({150.f,50.f});
      box2.setSize({150.f,50.f});
      box3.setSize({150.f,50.f});

      box1.setFillColor(sf::Color(0x33,0x33,0x33));
      box2.setFillColor(sf::Color(0x33,0x33,0x33));
      box3.setFillColor(sf::Color(0x33,0x33,0x33));

      box3.setPosition(sf::Vector2f{5.f,_sets.res_y-100.f});
      box2.setPosition(sf::Vector2f{5.f,_sets.res_y-160.f});
      box1.setPosition(sf::Vector2f{5.f,_sets.res_y-220.f});
      
      text1.setFont(Txt_menager->get_font());
      text2.setFont(Txt_menager->get_font());
      text3.setFont(Txt_menager->get_font());

      text1.setCharacterSize(20);
      text2.setCharacterSize(20);
      text3.setCharacterSize(20);

      text1.setString("NEW GAME");
      text2.setString("LOAD GAME");
      text3.setString("EXIT");

      text3.setPosition(sf::Vector2f{10.f,_sets.res_y-90.f});
      text2.setPosition(sf::Vector2f{10.f,_sets.res_y-150.f});
      text1.setPosition(sf::Vector2f{10.f,_sets.res_y-210.f});

      in_box.setSize(sf::Vector2f{_sets.res_x-10.f,50.f});
      in_box.setPosition(sf::Vector2f{5.f,_sets.res_y/2.f-100.f});
      in_box.setFillColor(sf::Color(0x13,0x03,0x63));

      in_text.setCharacterSize(20);
      in_text.setFont(Txt_menager->get_font());
      in_text.setPosition(sf::Vector2f{_sets.res_x/2.f-5.f,_sets.res_y/2.f-90.f});

      bool load_game{false};
      //menu loop
      while(window->isOpen()){
	sf::Event event;

	while(window->pollEvent(event)){
	  if(event.type == sf::Event::Closed)
	    window->close();
	  if (event.type == sf::Event::MouseButtonPressed) {
	    auto mouse_pos = window->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
	    if(box1.getGlobalBounds().contains(mouse_pos))
	      game();
	    else if(box2.getGlobalBounds().contains(mouse_pos))
	      load_game = true;
	      //game("test.txt");
	    else if(box3.getGlobalBounds().contains(mouse_pos))
	      window->close();
	  }
	  if (load_game && event.type == sf::Event::TextEntered){
	    if(event.text.unicode == 8)
	      in_text.setString(in_text.getString().substring(0,in_text.getString().getSize()-1));
	    else if(event.text.unicode > 47 && event.text.unicode < 123) 
	      in_text.setString(in_text.getString().toAnsiString()+static_cast<char>(event.text.unicode));

	  }
	  if(load_game && event.type == sf::Event::KeyPressed){
	    if(event.key.code == sf::Keyboard::Escape){
	      load_game = false;
	      in_text.setString("");
	    }
	    else if(event.key.code == sf::Keyboard::Enter){
	      game(in_text.getString().toAnsiString());
	      load_game = false;
	      in_text.setString("");
	    }
	  }
	}
	
	window->clear();
	if(load_game) {
	  window->draw(in_box);
	  in_text.setPosition(sf::Vector2f{_sets.res_x/2.f-in_text.getGlobalBounds().width/2.f,in_text.getPosition().y});
	  window->draw(in_text);
	}
	window->draw(box1);
	window->draw(box2);
	window->draw(box3);
	window->draw(text1);
	window->draw(text2);
	window->draw(text3);
	window->display();
      }
    }
    void game(const std::string& file_name = ""){

      bool run{true},sv_input{false};
      
      sf::RectangleShape in_box;
      sf::Text in_text,FPS_box;
      in_box.setSize(sf::Vector2f{_sets.res_x-10.f,50.f});
      in_box.setPosition(sf::Vector2f{0.f,_sets.res_y/2.f-100.f});
      in_box.setFillColor(sf::Color(0x13,0x03,0x63));

      in_text.setCharacterSize(20);
      in_text.setFont(Txt_menager->get_font());
      in_text.setPosition(sf::Vector2f{_sets.res_x/2.f,_sets.res_y/2.f-90.f});
      
      sf::Vector2f resolution(_sets.res_x,_sets.res_y);
      sf::View camera(window->getDefaultView());
  
      theta::Chunk_Menager chunk({_sets.offset_pos_x,_sets.offset_pos_y},_sets.NUM_OF_COLLUMNS,_sets.NUM_OF_ROWS);
      if(!file_name.empty())
	theta::FileManager::load_map(std::string(_sets._dir_path)+file_name+".txt",chunk,*Txt_menager);
      else
	theta::RTG::Generate_Terrain(chunk, *Txt_menager);

      chunk.set_Active_Chunks(camera, resolution);

      theta::Player player(sf::Vector2f{ 0.f,0.f }, camera, *Txt_menager); 
      theta::Physic_Engine Physical_engine(chunk, theta::Physical_Object(player));

      
      while (window->isOpen() && run)
	{
	  sf::Event event;
	  theta::Side_of_Collision side_buffor[3] = {theta::Side_of_Collision::None,theta::Side_of_Collision::None,theta::Side_of_Collision::None};
	  uint8_t i_sb{0};
	  auto cam_vec = camera.getCenter()-window->getDefaultView().getCenter();
	  while (window->pollEvent(event))
	    {
	      if (event.type == sf::Event::Closed)
                window->close();
	      if(!sv_input){
		if (event.type == sf::Event::MouseButtonPressed) {
		  auto mouse_pos = window->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
		  if (player.is_Range(mouse_pos)) {
		    if (event.mouseButton.button == sf::Mouse::Button::Left) chunk.destroy_block_at(mouse_pos,true);
		    else {
		      mouse_pos.x = floorf(mouse_pos.x) - static_cast<int>(mouse_pos.x) % 32;
		      mouse_pos.y = floorf(mouse_pos.y) - static_cast<int>(mouse_pos.y) % 32;
		      auto tmp = player.get_item(false);
		      if(tmp != theta::block_type::None)
			if(chunk.insert(std::make_shared<theta::Block>(mouse_pos, *Txt_menager,tmp)))
			  player.get_item();
		    }
		  }
		}
		if (event.type == sf::Event::KeyPressed) {
		  if (event.key.code == sf::Keyboard::A
		      && player.get_collision_points(theta::Side_of_Collision::Left).x > _sets.border_l
		      && i_sb < 3)
		    {side_buffor[i_sb] = theta::Side_of_Collision::Left; ++i_sb;}
		  else if (event.key.code == sf::Keyboard::D
			   && player.get_collision_points(theta::Side_of_Collision::Right).x < _sets.border_r 
			   && i_sb < 3)
		    { side_buffor[i_sb] = theta::Side_of_Collision::Right; ++i_sb;}
		  else if (event.key.code == sf::Keyboard::W
			   && i_sb < 3)
		    { side_buffor[i_sb] = theta::Side_of_Collision::Top; ++i_sb;}
		  else if(event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5)
		    player.choose_inv_slot(static_cast<size_t>(event.key.code - sf::Keyboard::Num1));
		  else if(event.key.code == sf::Keyboard::Escape){
		    in_box.setPosition(in_box.getPosition() + cam_vec);
		    in_text.setPosition(in_text.getPosition() + cam_vec);
		    sv_input = true;
		    
		  }
		}
	      }
	      else{
		if (event.type == sf::Event::TextEntered){
		  if(event.text.unicode == 8)
		    in_text.setString(in_text.getString().substring(0,in_text.getString().getSize()-1));
		  else if(event.text.unicode > 47 && event.text.unicode < 123)
		    in_text.setString(in_text.getString().toAnsiString()+static_cast<char>(event.text.unicode));
		}
		if(event.type == sf::Event::KeyPressed){
		  if(event.key.code == sf::Keyboard::Escape){
		    run = false;
		  }
		  else if(event.key.code == sf::Keyboard::Enter){
		    theta::FileManager::save_map(std::string(_sets._dir_path)+in_text.getString().toAnsiString()+".txt",chunk.get_map());
		    run = false;
		  }
		}
	      }
	    }
	  if(!sv_input){
	    Physical_engine.object_moved(std::span{side_buffor});
	    Physical_engine.run();
	  }
	  window->clear(sf::Color::Blue);
	  window->setView(camera);
	  chunk.set_Active_Chunks(camera, resolution);
	  for(const auto& x : chunk.get_Active_Chunks()) {
	    if(x == nullptr) throw std::runtime_error("Entering to not created chunk");
	    for(const auto& it : x->Draw_Chunk())
	      window->draw(*it);
	  }
	  player.update_inv_slot(*Txt_menager);
	  window->draw(player);
	  if(sv_input) {
	    window->draw(in_box);
	    in_text.setPosition(sf::Vector2f{_sets.res_x/2.f-in_text.getGlobalBounds().width/2.f + cam_vec.x
					     ,in_text.getPosition().y});
	    window->draw(in_text);
	  }
	  window->display();;
	}
      window->setView(window->getDefaultView());
    }
  public:
    app(settings&& setting) : _sets(setting) {
      window = new sf::RenderWindow(sf::VideoMode(_sets.res_x, _sets.res_y), "Theta Project");
      window->setFramerateLimit(60);
      Txt_menager = new Texture_Menager(_sets._dir_path);
    }
    void run(){
      menu();
    }
  };
}
#endif 
