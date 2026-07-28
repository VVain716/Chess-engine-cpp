#include "board.hpp"

const Pixel blackSquare = newPixel(153, 87, 81, 255);
const Pixel whiteSquare = newPixel(241, 233, 232, 255);

bool drawBoard(SDL_Renderer *renderer, int width, int height) {
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
  return true;
}
