#ifndef MOVES_HPP
#define MOVES_HPP

#include "board.hpp"

bool king_check(Board *board, int kingpos); // kingpos is an integer between 0 and 63
vector<int> get_legal_moves(int pos, Board *board);



#endif
