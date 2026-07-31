#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "board.hpp"
#include "pieces.hpp"
#include "animate.hpp"
#include "moves.hpp"
#include "notate.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

const int white_king_starting = 7*8+4;
const int black_king_starting = 4;
const int white_left_rook_starting = 7*8;
const int black_left_rook_starting = 0;
const int white_right_rook_starting = 7*8+7;
const int black_right_rook_starting = 7;


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
  
  // max refresh rate 
  SDL_SetRenderVSync(renderer, 1);
  

  // load images 
  White::load_images(renderer);
  Black::load_images(renderer);
  Result::load_images(renderer);
  // initialize board 
  Board *board = new Board();
  Coordinate *coord = new Coordinate();
  // intialize clicked variables 
  int clicked_pos = -1;
  std::vector<int> legal_moves;
  
  // get white and black king positions
  int white_king = 7*8+4;
  int black_king = 4;

  // start with white's move 
  bool white_move = true;


  // RUNNING LOOP
  while (true) {
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
      // square that has been clicked is already been stored 
      if (row*8+col == clicked_pos) {continue;}

      // it is white's move 
      if (white_move) {
        // legal move
        if (std::find(legal_moves.begin(), legal_moves.end(), row*8+col) != legal_moves.end()) {
          // quickly check if the white king has moved 
          bool castled = false;
          if (board->get_piece(clicked_pos) == WHITE_KING) {
            // check if the move is a kingside or a queenside castle
            if (clicked_pos + 2 == row*8+col) {
              // kingside castle
              coord->add_kingside(board, white_move);
              ExecuteCastle::white_kingside_castle(renderer, board, WINDOW_WIDTH, WINDOW_HEIGHT);
              castled = true;
            }
            else if (clicked_pos - 2 == row*8+col) {
              // queenside castle
              coord->add_queenside(board, white_move);
              ExecuteCastle::white_queenside_castle(renderer, board, WINDOW_WIDTH, WINDOW_HEIGHT);
              castled = true;
            }
            white_king = row*8+col;
            board->white_king_moved = true;
          }
          
          // check whether either of the rooks moved, which would thus prevent castling
          else if (clicked_pos == white_left_rook_starting) {
            board->white_left_rook_moved = true;
          }

          else if (clicked_pos == white_right_rook_starting) {
            board->white_right_rook_moved = true;
          }
          
          if (!castled) {
            coord->add_move(board, clicked_pos, row*8+col);
            animate_move(renderer, clicked_pos, row*8+col, board, WINDOW_WIDTH, WINDOW_HEIGHT);
          }
          legal_moves.clear();
          clicked_pos = -1;
          // set it to blacks move 
          white_move = !white_move;
        }

        // black piece 
        else if (board->get_piece(row*8+col) == EMPTY || board->get_piece(row*8+col) > WHITE_KING) {
          legal_moves.clear();
          clicked_pos = row*8+col;
          legal_moves.push_back(clicked_pos);
        }

        // different white piece 
        else {
          clicked_pos = row*8+col;
          legal_moves.clear();
          legal_moves = get_legal_moves(clicked_pos, board, white_king, black_king);
          legal_moves.push_back(clicked_pos);

          // check if piece clicked is the white king
          if (board->get_piece(clicked_pos) == WHITE_KING && CheckCastle::white_kingside_castle(board)) {
            legal_moves.push_back(clicked_pos + 2); // add legal move for white to kingside castle
          }

          if (board->get_piece(clicked_pos) == WHITE_KING && CheckCastle::white_queenside_castle(board)) {
            legal_moves.push_back(clicked_pos - 2); // add legal move for white to queenside castle
          }
        }
      }


      // black to move 
      else {
        // legal move 
        if (std::find(legal_moves.begin(), legal_moves.end(), row*8+col) != legal_moves.end()) {
          
          // quickly check if black king has moved 
          bool castled = false;
          if (board->get_piece(clicked_pos) == BLACK_KING) {
            if (clicked_pos + 2 == row*8+col) {
              // kingside castle
              coord->add_kingside(board, white_move);
              ExecuteCastle::black_kingside_castle(renderer, board, WINDOW_WIDTH, WINDOW_HEIGHT);
              castled = true;
            }

            else if (clicked_pos - 2 == row*8+col) {
              // queenside castle
              coord->add_queenside(board, white_move);
              ExecuteCastle::black_queenside_castle(renderer, board, WINDOW_WIDTH, WINDOW_HEIGHT);
              castled = true;
            }
            
            black_king = row*8+col;
            board->black_king_moved = true;
          }
          
          // check if either of the rooks have moved which would prevent castling
          else if (clicked_pos == black_left_rook_starting) {
            board->black_left_rook_moved = true;
          }

          else if (clicked_pos == black_right_rook_starting) {
            board->black_right_rook_moved = true;
          }

          if (!castled){
            coord->add_move(board, clicked_pos, row*8+col);
            animate_move(renderer, clicked_pos, row*8+col, board, WINDOW_WIDTH, WINDOW_HEIGHT);
          }
          legal_moves.clear();
          clicked_pos = -1;
          white_move = !white_move; // set it to black to move 
        }

        // square clicked is empty or contains a white piece
        else if (board->get_piece(row*8+col) <= WHITE_KING) {
          legal_moves.clear();
          clicked_pos = row*8+col;
          legal_moves.push_back(clicked_pos);
        }

        // other black piece 
        else {
          clicked_pos = row*8+col;
          legal_moves.clear();
          legal_moves = get_legal_moves(clicked_pos, board, white_king, black_king); 
          legal_moves.push_back(clicked_pos);

          // check for kingside and queenside castle possibilities
          if (board->get_piece(clicked_pos) == BLACK_KING && CheckCastle::black_kingside_castle(board)) {
            legal_moves.push_back(clicked_pos + 2); // kingside castle
          }

          if (board->get_piece(clicked_pos) == BLACK_KING && CheckCastle::black_queenside_castle(board)){
            legal_moves.push_back(clicked_pos - 2);
          }
        }
      } 

    }
    // reset renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // draws grid
    board->drawBoard(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, legal_moves);
    
    // check if there is a check for white 
    if (white_move && king_check(board, white_king)) {
      board->check(renderer, white_king, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
    // check if black is being checked 
    else if (!white_move && king_check(board, black_king)) {
      board->check(renderer, black_king, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    if (checkmate(board, white_move, white_king, black_king)) {
      if (!white_move) {
        coord->white_win();
      }
      else {
        coord->black_win();
      }
      std::cout << coord->get_moves() << std::endl;
      break;
    }
    else if (stalemate(board, white_move, white_king, black_king)) {
      coord->draw();
      std::cout << coord->get_moves() << std::endl;
      break;
    }

    else {
      // game is still going on
      board->render_board(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    SDL_RenderPresent(renderer);
  }

  // clean up memory
  White::destroy_images();
  Black::destroy_images();
  Result::destroy_images();
  delete board;
  delete coord;
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
}
