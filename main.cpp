#include"app.h"

constexpr uint8_t NUM_OF_COLLUMNS{ 32 };
constexpr uint8_t NUM_OF_ROWS{ 24 };
constexpr float offset_pos_x = -512.f * NUM_OF_COLLUMNS;
constexpr float offset_pos_y = -512.f * NUM_OF_ROWS;
constexpr float border_r = offset_pos_x + (NUM_OF_COLLUMNS-2)*1024.f; 
constexpr float border_l = offset_pos_x + 2*1024.f;

constexpr std::string_view file_path = __FILE__ ;
constexpr std::string_view _dir_path = file_path.substr(0,file_path.size()-8LLU);


int main(){
  theta::app app(
		 {NUM_OF_COLLUMNS,
		  NUM_OF_ROWS,
		  offset_pos_x,
		  offset_pos_y,
		  border_r,
		  border_l,
		  _dir_path,
		  1920.f,
		  1080.f
                  });
  app.run();
  return 0;
}
