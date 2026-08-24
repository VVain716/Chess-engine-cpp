#include <iostream>
#include <vector>
#include <optional>
#include <algorithm>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "core/Types.hpp"
#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/MoveGen.hpp"
#include "core/Notation.hpp"
#include "core/Eval.hpp"
#include "core/Search.hpp"
#include "ui/TextureManager.hpp"
#include "ui/ChessRenderer.hpp"
#include "ui/Animation.hpp"

namespace {
constexpr int DEFAULT_BOARD_SIZE  = 800;
constexpr int SIDEBAR_WIDTH       = 320;
constexpr int DEFAULT_TOTAL_WIDTH = DEFAULT_BOARD_SIZE + SIDEBAR_WIDTH;
constexpr int DEFAULT_TOTAL_HEIGHT = DEFAULT_BOARD_SIZE;
constexpr int AI_SEARCH_DEPTH     = 4;

// Simple custom line rendering helper for graph
void draw_thick_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, float thickness) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) return;

    float nx = -dy / len * (thickness / 2.0f);
    float ny =  dx / len * (thickness / 2.0f);

    SDL_Vertex vertices[4];
    vertices[0] = SDL_Vertex{SDL_FPoint{x1 + nx, y1 + ny}, SDL_FColor{1.0f, 1.0f, 1.0f, 1.0f}, SDL_FPoint{0, 0}};
    vertices[1] = SDL_Vertex{SDL_FPoint{x1 - nx, y1 - ny}, SDL_FColor{1.0f, 1.0f, 1.0f, 1.0f}, SDL_FPoint{0, 0}};
    vertices[2] = SDL_Vertex{SDL_FPoint{x2 - nx, y2 - ny}, SDL_FColor{1.0f, 1.0f, 1.0f, 1.0f}, SDL_FPoint{0, 0}};
    vertices[3] = SDL_Vertex{SDL_FPoint{x2 + nx, y2 + ny}, SDL_FColor{1.0f, 1.0f, 1.0f, 1.0f}, SDL_FPoint{0, 0}};

    int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
}

void render_sidebar(SDL_Renderer* renderer, int board_w, int total_w, int height,
                    int current_eval, const std::vector<int>& eval_history) {
    float side_x = static_cast<float>(board_w);
    float side_w = static_cast<float>(total_w - board_w);
    float side_h = static_cast<float>(height);

    // 1. Sidebar Background
    SDL_FRect bg_rect{side_x, 0, side_w, side_h};
    SDL_SetRenderDrawColor(renderer, 24, 26, 27, 255); // Dark sleek background
    SDL_RenderFillRect(renderer, &bg_rect);

    // Border separator line
    SDL_FRect sep_line{side_x, 0, 2.0f, side_h};
    SDL_SetRenderDrawColor(renderer, 55, 58, 60, 255);
    SDL_RenderFillRect(renderer, &sep_line);

    // 2. Vertical Eval Bar on the left edge of sidebar
    float bar_x = side_x + 16.0f;
    float bar_y = 20.0f;
    float bar_w = 26.0f;
    float bar_h = side_h - 40.0f;

    // Background of bar (Black advantage: dark grey)
    SDL_FRect bar_bg{bar_x, bar_y, bar_w, bar_h};
    SDL_SetRenderDrawColor(renderer, 40, 42, 44, 255);
    SDL_RenderFillRect(renderer, &bar_bg);

    // Compute white height percentage (clamped between -1000cp and +1000cp)
    float clamped_eval = std::clamp(static_cast<float>(current_eval), -1000.0f, 1000.0f);
    float win_prob = 1.0f / (1.0f + std::pow(10.0f, -clamped_eval / 400.0f)); // Sigmoid-like scaling
    float white_bar_h = bar_h * win_prob;

    // White Advantage Part (rendered from bottom up)
    SDL_FRect white_bar{bar_x, bar_y + (bar_h - white_bar_h), bar_w, white_bar_h};
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &white_bar);

    // Eval Bar Outline
    SDL_SetRenderDrawColor(renderer, 80, 84, 88, 255);
    SDL_RenderRect(renderer, &bar_bg);

    // Midline on Eval Bar (0.0 level)
    SDL_FRect mid_line{bar_x, bar_y + bar_h / 2.0f - 1.0f, bar_w, 2.0f};
    SDL_SetRenderDrawColor(renderer, 134, 151, 110, 255);
    SDL_RenderFillRect(renderer, &mid_line);

    // 3. Live Eval Graph Container (occupies full remaining height)
    float content_x = bar_x + bar_w + 16.0f;
    float content_w = side_w - (bar_w + 48.0f);
    float graph_y   = bar_y;
    float graph_h   = bar_h;

    SDL_FRect graph_card{content_x, graph_y, content_w, graph_h};
    SDL_SetRenderDrawColor(renderer, 30, 32, 34, 255);
    SDL_RenderFillRect(renderer, &graph_card);
    SDL_SetRenderDrawColor(renderer, 55, 58, 60, 255);
    SDL_RenderRect(renderer, &graph_card);

    // Zero-line inside graph (0.0 cp level)
    float zero_y = graph_y + graph_h / 2.0f;
    SDL_FRect graph_zero_line{content_x + 4.0f, zero_y, content_w - 8.0f, 1.0f};
    SDL_SetRenderDrawColor(renderer, 70, 75, 80, 200);
    SDL_RenderFillRect(renderer, &graph_zero_line);

    // Plot Eval History Curve
    if (!eval_history.empty()) {
        size_t count = eval_history.size();
        float x_step = (count > 1) ? (content_w - 24.0f) / static_cast<float>(count - 1) : 0.0f;

        auto eval_to_y = [&](int ev) -> float {
            float val = std::clamp(static_cast<float>(ev), -800.0f, 800.0f);
            // Positive eval (White) goes up, negative (Black) goes down
            return zero_y - (val / 800.0f) * (graph_h / 2.0f - 20.0f);
        };

        for (size_t i = 0; i < count; ++i) {
            float pt_x = content_x + 12.0f + static_cast<float>(i) * x_step;
            float pt_y = eval_to_y(eval_history[i]);

            if (i > 0) {
                float prev_x = content_x + 12.0f + static_cast<float>(i - 1) * x_step;
                float prev_y = eval_to_y(eval_history[i - 1]);

                // Line color: Green if favoring White, Red/Orange if favoring Black
                if (eval_history[i] >= 0) {
                    SDL_SetRenderDrawColor(renderer, 134, 190, 110, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 226, 80, 60, 255);
                }
                draw_thick_line(renderer, prev_x, prev_y, pt_x, pt_y, 2.5f);
            }

            // Dot on each point
            SDL_FRect pt_rect{pt_x - 3.0f, pt_y - 3.0f, 6.0f, 6.0f};
            SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
            SDL_RenderFillRect(renderer, &pt_rect);
        }
    }
}

} // namespace

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Chess Engine - Play vs Computer",
                                     DEFAULT_TOTAL_WIDTH, DEFAULT_TOTAL_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);

    chess::ui::TextureManager texture_manager;
    if (!texture_manager.load_all_textures(renderer)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Some textures could not be loaded.");
    }

    chess::ui::ChessRenderer chess_renderer;
    chess::Board board;
    chess::MoveHistory history;

    std::optional<chess::Square> selected_sq = std::nullopt;
    std::vector<chess::Move> legal_moves_for_selected;
    std::vector<chess::Square> highlighted_dest_squares;
    chess::ui::ResultType game_result = chess::ui::ResultType::None;

    std::vector<int> eval_history;
    eval_history.push_back(chess::Search::minmax_eval(board)); // Start eval (0 cp)

    int current_eval = 0;
    bool ai_thinking = false;
    bool is_running = true;
    SDL_Event event;

    std::cout << "========================================\n";
    std::cout << "  CHESS ENGINE: HUMAN (WHITE) VS AI    \n";
    std::cout << "========================================\n";

    int total_w = DEFAULT_TOTAL_WIDTH;
    int total_h = DEFAULT_TOTAL_HEIGHT;
    int board_w = DEFAULT_BOARD_SIZE;

    auto render_all_frame = [&]() {
        SDL_GetRenderOutputSize(renderer, &total_w, &total_h);
        board_w = std::min(total_h, total_w - 200);
        if (board_w <= 0) board_w = DEFAULT_BOARD_SIZE;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Highlight king if in check
        std::optional<chess::Square> check_sq = std::nullopt;
        if (chess::MoveGen::is_in_check(board, board.side_to_move())) {
            check_sq = board.king_square(board.side_to_move());
        }

        // Draw Chessboard & Pieces
        chess_renderer.draw_board(renderer, board_w, total_h, selected_sq, highlighted_dest_squares, check_sq);
        chess_renderer.render_pieces(renderer, board, texture_manager, board_w, total_h);

        if (game_result != chess::ui::ResultType::None) {
            chess_renderer.render_result(renderer, texture_manager, game_result, board_w, total_h);
        }

        // Draw Sidebar & Live Eval Graph
        render_sidebar(renderer, board_w, total_w, total_h, current_eval, eval_history);

        SDL_RenderPresent(renderer);
    };

    while (is_running) {
        SDL_GetRenderOutputSize(renderer, &total_w, &total_h);
        board_w = std::min(total_h, total_w - 200);
        if (board_w <= 0) board_w = DEFAULT_BOARD_SIZE;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                       game_result == chess::ui::ResultType::None &&
                       board.side_to_move() == chess::Color::White &&
                       !ai_thinking) {

                auto clicked_sq_opt = chess::ui::ChessRenderer::square_from_pixel(
                    event.button.x, event.button.y, board_w, total_h);

                if (!clicked_sq_opt.has_value()) {
                    continue;
                }

                chess::Square clicked_sq = *clicked_sq_opt;

                // 1. Try to make human move if clicked on a legal destination
                bool move_executed = false;
                if (selected_sq.has_value()) {
                    auto it = std::find_if(legal_moves_for_selected.begin(), legal_moves_for_selected.end(),
                                           [clicked_sq](const chess::Move& m) { return m.to == clicked_sq; });

                    if (it != legal_moves_for_selected.end()) {
                        chess::Move chosen_move = *it;

                        history.record_move(board, chosen_move);
                        chess::ui::Animation::animate_move(renderer, board, chosen_move,
                                                           texture_manager, chess_renderer, board_w, total_h);
                        board.make_move(chosen_move);

                        selected_sq = std::nullopt;
                        legal_moves_for_selected.clear();
                        highlighted_dest_squares.clear();
                        move_executed = true;

                        current_eval = chess::Search::minmax_eval(board, AI_SEARCH_DEPTH);
                        eval_history.push_back(current_eval);

                        // Render immediately so the new board and eval graph are visible
                        render_all_frame();

                        // Check game ending conditions
                        if (chess::MoveGen::is_checkmate(board)) {
                            game_result = chess::ui::ResultType::WhiteWin;
                            history.record_white_win();
                            std::cout << "\n=== Game Over (Checkmate - You Win!) ===\n" << history.get_notation() << std::endl;
                        } else if (chess::MoveGen::is_draw(board)) {
                            game_result = chess::ui::ResultType::Draw;
                            history.record_draw();
                            std::cout << "\n=== Game Over (Draw by Stalemate / Insufficient Material) ===\n" << history.get_notation() << std::endl;
                        }
                    }
                }

                // 2. Select / switch piece if clicked on a friendly piece
                if (!move_executed) {
                    chess::Piece clicked_piece = board.get_piece(clicked_sq);
                    if (clicked_piece != chess::Piece::None && chess::piece_color(clicked_piece) == chess::Color::White) {
                        selected_sq = clicked_sq;
                        legal_moves_for_selected = chess::MoveGen::generate_legal_moves(board, clicked_sq);
                        highlighted_dest_squares.clear();
                        for (const auto& m : legal_moves_for_selected) {
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

        // Render current frame (including board and eval graphs)
        render_all_frame();

        // 3. AI Turn Execution (Computer plays as Black)
        if (board.side_to_move() == chess::Color::Black &&
            game_result == chess::ui::ResultType::None &&
            !ai_thinking) {

            ai_thinking = true;

            // Make sure the entire frame with graphs is fully rendered and shown before minimax search
            render_all_frame();

            // Compute best move using Minimax
            chess::SearchResult search_res = chess::Search::search(board, AI_SEARCH_DEPTH);

            if (search_res.best_move.is_valid()) {
                double score = search_res.score;
                score /= -100;
                std::cout << "[AI Move]: " << chess::Notation::format_move(board, search_res.best_move)
                          << " (Eval: " << score
                          << " cp, Nodes: " << search_res.nodes << ")" << std::endl;

                history.record_move(board, search_res.best_move);
                chess::ui::Animation::animate_move(renderer, board, search_res.best_move,
                                                   texture_manager, chess_renderer, board_w, total_h);
                board.make_move(search_res.best_move);

                current_eval = -search_res.score;
                eval_history.push_back(current_eval);

                // Render immediately after making the move
                render_all_frame();

                if (chess::MoveGen::is_checkmate(board)) {
                    game_result = chess::ui::ResultType::BlackWin;
                    history.record_black_win();
                    std::cout << "\n=== Game Over (Checkmate - AI Wins!) ===\n" << history.get_notation() << std::endl;
                } else if (chess::MoveGen::is_draw(board)) {
                    game_result = chess::ui::ResultType::Draw;
                    history.record_draw();
                    std::cout << "\n=== Game Over (Draw by Stalemate / Insufficient Material) ===\n" << history.get_notation() << std::endl;
                }
            }

            ai_thinking = false;
        }
    }

    texture_manager.destroy();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
