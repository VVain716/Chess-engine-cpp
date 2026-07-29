#ifndef BOARD_HPP
#define BOARD_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "pixel.hpp"

#define EMPTY 0

#define WHITE_PAWN 1
#define WHITE_KNIGHT 2
#define WHITE_BISHOP 3
#define WHITE_ROOK 4
#define WHITE_QUEEN 5
#define WHITE_KING 6

#define BLACK_PAWN 7
#define BLACK_KNIGHT 8
#define BLACK_BISHOP 9
#define BLACK_ROOK 10
#define BLACK_QUEEN 11
#define BLACK_KING 12


bool drawBoard(SDL_Renderer *renderer, int width, int height);

class Board {
  // 0 = empty, 1 = white pawn, 2 = white knight, 3 = white bishop, 4 = white rook, 5 = white queen, 6 = white king, add 6 to each piece value to make it black
private:
  int board[8][8] = 
  { {10, 8, 9, 11, 12, 9, 8, 10},
    {7, 7, 7, 7, 7, 7, 7, 7},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {4, 2, 3, 5, 6, 3, 2, 4}
  };
public:
  bool render_board(SDL_Renderer *renderer, int width, int height);
  int get_piece(int val);
  bool set_piece(int val, int new_piece);
};
#endif
