#ifndef PIECES_HPP
#define PIECES_HPP

#ifndef WHITE_PATH
#define WHITE_PATH "../pieces/white"
#endif

#ifndef BLACK_PATH
#define BLACK_PATH "../pieces/black"
#endif

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>


// load all the pieces into memory
namespace White {
  extern SDL_Texture *pawn;
  extern SDL_Texture *knight;
  extern SDL_Texture *bishop;
  extern SDL_Texture *rook;
  extern SDL_Texture *queen;
  extern SDL_Texture *king;
  
  bool load_images(SDL_Renderer *renderer);
  bool destroy_images();
}

namespace Black {
  extern SDL_Texture *pawn;
  extern SDL_Texture *knight;
  extern SDL_Texture *bishop;
  extern SDL_Texture *rook;
  extern SDL_Texture *queen;
  extern SDL_Texture *king;
  
  bool load_images(SDL_Renderer *renderer);
  bool destroy_images();
}

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
};

#endif
