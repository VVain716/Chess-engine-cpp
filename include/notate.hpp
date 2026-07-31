#ifndef NOTATE_HPP
#define NOTATE_HPP

#include "board.hpp"
#include "moves.hpp"
#include <string> 

#define ESCAPE "\033[0m"
#define RED "\033[91;1m"
#define GREEN "\033[92;1m"
#define YELLOW "\033[93;1m"

class Coordinate {
private:
  std::string s; 
  int move_number = 1;
public:
  std::string get_moves();
  std::string notate_pos(int pos);
  bool add_move(Board *board, int src, int dest); // ex Ng1-f3, Ng1xf3
  bool add_kingside(Board *board, bool is_white);
  bool add_queenside(Board *board, bool is_white);
  bool white_win();
  bool black_win();
  bool draw();
};


#endif
