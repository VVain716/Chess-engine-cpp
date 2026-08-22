#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

#include "core/Board.hpp"
#include "core/Eval.hpp"
#include "core/Search.hpp"
#include "core/Move.hpp"
#include "core/MoveGen.hpp"
#include "core/Notation.hpp"
#include "core/Types.hpp"
#include "ui/Animation.hpp"
#include "ui/ChessRenderer.hpp"
#include "ui/TextureManager.hpp"

namespace {
constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 800;
} // namespace

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s",
                 SDL_GetError());
    return 1;
  }

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (!SDL_CreateWindowAndRenderer("Chess Engine", DEFAULT_WINDOW_WIDTH,
                                   DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE,
                                   &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create window and renderer: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_SetRenderVSync(renderer, 1);

  chess::ui::TextureManager texture_manager;
  if (!texture_manager.load_all_textures(renderer)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Some textures could not be loaded.");
  }

  chess::ui::ChessRenderer chess_renderer;
  chess::Board board;
  chess::MoveHistory history;

  std::optional<chess::Square> selected_sq = std::nullopt;
  std::vector<chess::Move> legal_moves_for_selected;
  std::vector<chess::Square> highlighted_dest_squares;
  chess::ui::ResultType game_result = chess::ui::ResultType::None;

  bool is_running = true;
  SDL_Event event;

  while (is_running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        is_running = false;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                 game_result == chess::ui::ResultType::None) {
        int win_w = DEFAULT_WINDOW_WIDTH;
        int win_h = DEFAULT_WINDOW_HEIGHT;
        SDL_GetRenderOutputSize(renderer, &win_w, &win_h);

        auto clicked_sq_opt = chess::ui::ChessRenderer::square_from_pixel(
            event.button.x, event.button.y, win_w, win_h);

        if (!clicked_sq_opt.has_value()) {
          continue;
        }

        chess::Square clicked_sq = *clicked_sq_opt;

        // 1. Try to make a move if a piece is selected and the clicked square
        // is a legal destination
        bool move_executed = false;
        if (selected_sq.has_value()) {
          auto it = std::find_if(legal_moves_for_selected.begin(),
                                 legal_moves_for_selected.end(),
                                 [clicked_sq](const chess::Move &m) {
                                   return m.to == clicked_sq;
                                 });

          if (it != legal_moves_for_selected.end()) {
            chess::Move chosen_move = *it;

            history.record_move(board, chosen_move);
            chess::ui::Animation::animate_move(renderer, board, chosen_move,
                                               texture_manager, chess_renderer,
                                               win_w, win_h);
            board.make_move(chosen_move);

            // Clear selection
            selected_sq = std::nullopt;
            legal_moves_for_selected.clear();
            highlighted_dest_squares.clear();
            move_executed = true;

            // Check game ending conditions
            if (chess::MoveGen::is_checkmate(board)) {
              if (board.side_to_move() == chess::Color::White) {
                game_result = chess::ui::ResultType::BlackWin;
                history.record_black_win();
              } else {
                game_result = chess::ui::ResultType::WhiteWin;
                history.record_white_win();
              }
              std::cout << "\n=== Game Over (Checkmate) ===\n"
                        << history.get_notation() << std::endl;
            } else if (chess::MoveGen::is_draw(board)) {
              game_result = chess::ui::ResultType::Draw;
              history.record_draw();
              std::cout << "\n=== Game Over (Draw by Stalemate / Insufficient Material) ===\n"
                        << history.get_notation() << std::endl;
            }
          }
        }

        // 2. If move was not executed, handle selecting or switching piece
        if (!move_executed) {
          chess::Piece clicked_piece = board.get_piece(clicked_sq);
          if (clicked_piece != chess::Piece::None &&
              chess::piece_color(clicked_piece) == board.side_to_move()) {
            selected_sq = clicked_sq;
            legal_moves_for_selected =
                chess::MoveGen::generate_legal_moves(board, clicked_sq);
            highlighted_dest_squares.clear();
            for (const auto &m : legal_moves_for_selected) {
              highlighted_dest_squares.push_back(m.to);
            }
          } else {
            selected_sq = std::nullopt;
            legal_moves_for_selected.clear();
            highlighted_dest_squares.clear();
          }
        }
      }
    }

    // Rendering frame
    int width = DEFAULT_WINDOW_WIDTH;
    int height = DEFAULT_WINDOW_HEIGHT;
    SDL_GetRenderOutputSize(renderer, &width, &height);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Highlight king if in check
    std::optional<chess::Square> check_sq = std::nullopt;
    if (chess::MoveGen::is_in_check(board, board.side_to_move())) {
      check_sq = board.king_square(board.side_to_move());
    }

    chess_renderer.draw_board(renderer, width, height, selected_sq,
                              highlighted_dest_squares, check_sq);
    chess_renderer.render_pieces(renderer, board, texture_manager, width,
                                 height);

    if (game_result != chess::ui::ResultType::None) {
      chess_renderer.render_result(renderer, texture_manager, game_result,
                                   width, height);
    }
    SDL_RenderPresent(renderer);
    std::cout << chess::Notation::format_move(board, chess::Search::get_best_move(board, 3)) << std::endl;
  }

  // Cleanup managed via RAII and SDL destruction
  texture_manager.destroy();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
