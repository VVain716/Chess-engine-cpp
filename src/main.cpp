#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "board.hpp"
#include "pieces.hpp"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

int main() {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;
  

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL");
    return 3;
  }  
  if (!SDL_CreateWindowAndRenderer("chessboard", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer");
  }
  White::load_images(renderer);
  Black::load_images(renderer);
  Board *board = new Board();
  while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
      break;
    }

    // running loop
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // draws board
    drawBoard(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    board->render_board(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_RenderPresent(renderer);
  }
  White::destroy_images();
  Black::destroy_images();
  delete board;
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
}
