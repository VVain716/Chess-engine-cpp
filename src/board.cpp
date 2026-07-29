#include "board.hpp"
#include "pieces.hpp"
const Pixel blackSquare = newPixel(153, 87, 81, 255);
const Pixel whiteSquare = newPixel(241, 233, 232, 255);
const Pixel clickedSquare = newPixel(134, 151, 110, 255);
bool drawBoard(SDL_Renderer *renderer, int width, int height, std::vector<int> clicked) {
  int squareWidth = width / 8;
  int squareHeight = height / 8;
  for (int i = 0; i < width; i += squareWidth) {
    for (int j = 0; j < height; j += squareHeight) {
      SDL_FRect tmp;
      SDL_FRect *rect = &tmp;
      rect->x = i;
      rect->y = j;
      rect->w = squareWidth;
      rect->h = squareHeight;
      // get the color
      if ((i / 100 + j / 100) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, blackSquare.red, blackSquare.green, blackSquare.blue, blackSquare.alpha);
      }
      else {
        SDL_SetRenderDrawColor(renderer, whiteSquare.red, whiteSquare.green, whiteSquare.blue, whiteSquare.alpha);
      }
      SDL_RenderFillRect(renderer, rect);
    }
  }
  for (int x : clicked) {
    int row = x / 8;
    int col = x % 8;
    SDL_FRect tmp;
    SDL_FRect *rect = &tmp;
    rect->x = col * squareWidth;
    rect->y = row * squareHeight;
    rect->w = squareWidth;
    rect->h = squareHeight;
    SDL_SetRenderDrawColor(renderer, clickedSquare.red, clickedSquare.green, clickedSquare.blue, clickedSquare.alpha);
    SDL_RenderFillRect(renderer, rect);
  }

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
