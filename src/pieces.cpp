#include "pieces.hpp"


namespace White {
  SDL_Texture *pawn = nullptr;
  SDL_Texture *knight = nullptr;
  SDL_Texture *bishop = nullptr;
  SDL_Texture *rook = nullptr;
  SDL_Texture *queen = nullptr;
  SDL_Texture *king = nullptr;
}

namespace Black {
  SDL_Texture *pawn = nullptr;
  SDL_Texture *knight = nullptr;
  SDL_Texture *bishop = nullptr;
  SDL_Texture *rook = nullptr;
  SDL_Texture *queen = nullptr;
  SDL_Texture *king = nullptr;
}

namespace Result {
  SDL_Texture *white_win = nullptr;
  SDL_Texture *black_win = nullptr;
  SDL_Texture *draw = nullptr;
}

SDL_Texture *get_texture(int val) {
  switch (val) {
    case 1:
      return White::pawn;
      break;
    case 2:
      return White::knight;
      break;
    case 3:
      return White::bishop;
      break;
    case 4:
      return White::rook;
      break;
    case 5:
      return White::queen;
      break;
    case 6:
      return White::king;
      break;
    case 7:
      return Black::pawn;
      break;
    case 8:
      return Black::knight;
      break;
    case 9:
      return Black::bishop;
      break;
    case 10:
      return Black::rook;
      break;
    case 11:
      return Black::queen;
      break;
    case 12:
      return Black::king;
      break;
    default:
      break;
  }
  return nullptr;
}

bool White::load_images(SDL_Renderer *renderer) {
  // expects pieces to be in file directory WHITE_PATH and pieces to be named pawn.png, bishop.png, etc.
  pawn = IMG_LoadTexture(renderer, WHITE_PATH "/pawn.png");
  bishop = IMG_LoadTexture(renderer, WHITE_PATH  "/bishop.png");
  knight = IMG_LoadTexture(renderer, WHITE_PATH  "/knight.png");
  rook = IMG_LoadTexture(renderer, WHITE_PATH  "/rook.png");
  queen = IMG_LoadTexture(renderer, WHITE_PATH  "/queen.png");
  king = IMG_LoadTexture(renderer, WHITE_PATH  "/king.png");
  return true;
}

bool Black::load_images(SDL_Renderer *renderer) {
  pawn = IMG_LoadTexture(renderer, BLACK_PATH  "/pawn.png");
  bishop = IMG_LoadTexture(renderer, BLACK_PATH  "/bishop.png");
  knight = IMG_LoadTexture(renderer, BLACK_PATH  "/knight.png");
  rook = IMG_LoadTexture(renderer, BLACK_PATH  "/rook.png");
  queen = IMG_LoadTexture(renderer, BLACK_PATH  "/queen.png");
  king = IMG_LoadTexture(renderer, BLACK_PATH  "/king.png");
  return true;
}

bool White::destroy_images() {
  SDL_DestroyTexture(pawn);
  SDL_DestroyTexture(bishop);
  SDL_DestroyTexture(knight);
  SDL_DestroyTexture(rook);
  SDL_DestroyTexture(queen);
  SDL_DestroyTexture(king);
  return true;
}

bool Black::destroy_images() {
  SDL_DestroyTexture(pawn);
  SDL_DestroyTexture(bishop);
  SDL_DestroyTexture(knight);
  SDL_DestroyTexture(rook);
  SDL_DestroyTexture(queen);
  SDL_DestroyTexture(king);
  return true;
}

bool Result::load_images(SDL_Renderer *renderer) {
  white_win = IMG_LoadTexture(renderer, RESULTS_PATH "/white-win.png");
  black_win = IMG_LoadTexture(renderer, RESULTS_PATH "/black-win.png");
  draw = IMG_LoadTexture(renderer, RESULTS_PATH "/draw.png");
  return true;
}

bool Result::destroy_images() {
  SDL_DestroyTexture(white_win);
  SDL_DestroyTexture(black_win);
  SDL_DestroyTexture(draw);
  return true;
}
