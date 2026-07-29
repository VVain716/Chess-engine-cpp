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
      SDL_Texture *s = get_texture(board[h][w]);
      if (s == nullptr) {
        continue;
      }
      SDL_RenderTexture(renderer, s, nullptr, rect);
    }
  }
  return true;
}

int Board::get_piece(int val) {
  int row = val / 8;
  int col = val % 8;
  return board[row][col];
}

bool Board::set_piece(int val, int new_piece) {
  if (new_piece > 12 || new_piece < 0) {return false;}
  int row = val / 8;
  int col = val % 8;
  board[row][col] = new_piece;
  return true;
}
