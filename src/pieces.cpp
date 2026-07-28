#include "pieces.hpp"


namespace White {
  SDL_Texture *pawn = nullptr;
  SDL_Texture *knight = nullptr;
  SDL_Texture *bishop = nullptr;
  SDL_Texture *rook = nullptr;
  SDL_Texture *queen = nullptr;
  SDL_Texture *king = nullptr;
  
  bool load_images();
  bool destroy_images();
}

namespace Black {
  SDL_Texture *pawn = nullptr;
  SDL_Texture *knight = nullptr;
  SDL_Texture *bishop = nullptr;
  SDL_Texture *rook = nullptr;
  SDL_Texture *queen = nullptr;
  SDL_Texture *king = nullptr;
  
  bool load_images();
  bool destroy_images();
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


bool Board::render_board(SDL_Renderer *renderer, int width, int height) {
  for (int h = 0; h < 8; h++) {
    for (int w = 0; w < 8; w++) {
      SDL_FRect tmp;
      SDL_FRect *rect = &tmp;
      rect->x = w * width / 8;
      rect->y = h * height / 8;
      rect->w = width / 8;
      rect->h = height / 8;
      // see what piece we have to render
      SDL_Texture *s = nullptr;
      switch (board[h][w]) {
        case 1:
          s = White::pawn;
          break;
        case 2:
          s = White::knight;
          break;
        case 3:
          s = White::bishop;
          break;
        case 4:
          s = White::rook;
          break;
        case 5:
          s = White::queen;
          break;
        case 6:
          s = White::king;
          break;
        case 7:
          s = Black::pawn;
          break;
        case 8:
          s = Black::knight;
          break;
        case 9:
          s = Black::bishop;
          break;
        case 10:
          s = Black::rook;
          break;
        case 11:
          s = Black::queen;
          break;
        case 12:
          s = Black::king;
          break;
        default:
          break;
      }
      if (s == nullptr) {
        continue;
      }
      SDL_RenderTexture(renderer, s, nullptr, rect);
    }
  }
  return true;
}




