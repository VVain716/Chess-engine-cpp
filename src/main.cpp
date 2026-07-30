#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "board.hpp"
#include "pieces.hpp"
#include "animate.hpp"
#include "moves.hpp"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

int main() {

  // INIT 
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
  
  SDL_SetRenderVSync(renderer, 1);

  White::load_images(renderer);
  Black::load_images(renderer);
  Board *board = new Board();
  int clicked_pos = -1;
  std::vector<int> legal_moves;

  // RUNNING LOOP
  while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
      break;
    }

    // get piece that is clicked
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
      double xPos = event.button.x;
      double yPos = event.button.y;
      int row = yPos / (WINDOW_HEIGHT / 8);
      int col = xPos / (WINDOW_WIDTH / 8);
      if (row*8+col == clicked_pos) {continue;}
      // square clicked is in the legal_moves of the piece that is clicked
      else if (std::find(legal_moves.begin(), legal_moves.end(), row*8+col) != legal_moves.end()) {
        animate_move(renderer, clicked_pos, row*8+col, board, WINDOW_WIDTH, WINDOW_HEIGHT);
        legal_moves.clear();
        clicked_pos = -1;
      }

      // square clicked is not a legal move 
      else {
        clicked_pos = row*8+col;
        legal_moves.clear();
        legal_moves = get_legal_moves(clicked_pos, board, 0, 0); //TODO: Update get_legal_moves to handle the white king and the black king
        legal_moves.push_back(clicked_pos);
      }
    }
    // reset renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // draws board
    drawBoard(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, legal_moves);
    board->render_board(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_RenderPresent(renderer);
  }
  White::destroy_images();
  Black::destroy_images();
  delete board;
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
}
