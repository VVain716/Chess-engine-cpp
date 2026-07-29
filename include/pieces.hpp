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

SDL_Texture *get_texture(int val);


#endif
