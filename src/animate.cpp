#include "animate.hpp"
#include "board.hpp"
#include "pixel.hpp"
#include "pieces.hpp"
#include <math.h>

bool animate_move(SDL_Renderer *renderer, int src, int dest, Board *board, int width, int height) {
  // initial drawing
  const int SPEED = 8;
  const int TICKS = SDL_GetPerformanceFrequency() / FPS;

  int src_row = src / 8; int src_col = src % 8; int dest_row = dest / 8; int dest_col = dest % 8;
  int piece = board->get_piece(src);
  SDL_Texture *s = get_texture(piece);
  if (s == nullptr) {
    return false;
  }
  board->set_piece(src, 0);
  SDL_RenderPresent(renderer);
  SDL_FRect tmp;
  SDL_FRect *curr = &tmp;
  curr->x = src_col * width / 8;
  curr->y = src_row * height / 8;
  curr->w = width / 8;
  curr->h = height / 8;
  int dest_x = dest_col * width / 8;
  int dest_y = dest_row * height / 8;

  double length = sqrt((dest_x - curr->x) * (dest_x - curr->x) + (dest_y - curr->y) * (dest_y - curr->y));
  double dy = (dest_y - curr->y) / length * SPEED;
  double dx = (dest_x - curr->x) / length * SPEED;

  while (fabs(dest_y - curr->y) > 1 || fabs(dest_x - curr->x) > 1) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // draws board
    drawBoard(renderer, width, height);
    board->render_board(renderer, width, height);

    int frame_begin = SDL_GetPerformanceCounter();
    curr->x += dx;
    curr->y += dy;
    SDL_RenderTexture(renderer, s, nullptr, curr);
    SDL_RenderPresent(renderer);
    int frame_end = SDL_GetPerformanceCounter();
    if (frame_end - frame_begin < TICKS) {
      int remaining = TICKS - (frame_end - frame_begin);

      int delay = (remaining * 1000.0) / SDL_GetPerformanceFrequency();

      SDL_Delay(delay);
    }
  }
  board->set_piece(dest, piece);
  return true;
}
