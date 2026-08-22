#include "ui/ChessRenderer.hpp"
#include <algorithm>

namespace chess::ui {

std::optional<Square> ChessRenderer::square_from_pixel(float x, float y, int width, int height) {
    if (width <= 0 || height <= 0) return std::nullopt;
    float sq_w = static_cast<float>(width) / 8.0f;
    float sq_h = static_cast<float>(height) / 8.0f;

    int col = static_cast<int>(x / sq_w);
    int row = static_cast<int>(y / sq_h);

    if (is_valid_square(row, col)) {
        return make_square(row, col);
    }
    return std::nullopt;
}

SDL_FRect ChessRenderer::square_to_rect(Square sq, int width, int height) {
    float sq_w = static_cast<float>(width) / 8.0f;
    float sq_h = static_cast<float>(height) / 8.0f;

    int row = square_row(sq);
    int col = square_col(sq);

    return SDL_FRect{
        static_cast<float>(col) * sq_w,
        static_cast<float>(row) * sq_h,
        sq_w,
        sq_h
    };
}

void ChessRenderer::draw_board(SDL_Renderer* renderer, int width, int height,
                              std::optional<Square> selected_sq,
                              const std::vector<Square>& highlighted_squares,
                              std::optional<Square> check_sq) const {
    float sq_w = static_cast<float>(width) / 8.0f;
    float sq_h = static_cast<float>(height) / 8.0f;

    // 1. Draw 8x8 checkerboard
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            SDL_FRect rect{
                static_cast<float>(col) * sq_w,
                static_cast<float>(row) * sq_h,
                sq_w,
                sq_h
            };

            const auto& c = ((row + col) % 2 != 0) ? theme_.dark_square : theme_.light_square;
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // 2. Draw check square
    if (check_sq.has_value() && is_valid_square(*check_sq)) {
        SDL_FRect rect = square_to_rect(*check_sq, width, height);
        SDL_SetRenderDrawColor(renderer, theme_.check_color.r, theme_.check_color.g, theme_.check_color.b, theme_.check_color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    // 3. Draw selected square
    if (selected_sq.has_value() && is_valid_square(*selected_sq)) {
        SDL_FRect rect = square_to_rect(*selected_sq, width, height);
        SDL_SetRenderDrawColor(renderer, theme_.highlight.r, theme_.highlight.g, theme_.highlight.b, theme_.highlight.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    // 4. Draw highlighted destination squares
    for (Square sq : highlighted_squares) {
        if (!is_valid_square(sq) || (selected_sq.has_value() && sq == *selected_sq)) continue;
        SDL_FRect rect = square_to_rect(sq, width, height);
        SDL_SetRenderDrawColor(renderer, theme_.highlight.r, theme_.highlight.g, theme_.highlight.b, 160);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void ChessRenderer::render_pieces(SDL_Renderer* renderer, const Board& board,
                                 const TextureManager& textures, int width, int height,
                                 std::optional<Square> skip_sq) const {
    for (Square sq = 0; sq < 64; ++sq) {
        if (skip_sq.has_value() && sq == *skip_sq) continue;
        Piece piece = board.get_piece(sq);
        if (piece == Piece::None) continue;

        SDL_Texture* tex = textures.get_piece_texture(piece);
        if (tex) {
            SDL_FRect rect = square_to_rect(sq, width, height);
            SDL_RenderTexture(renderer, tex, nullptr, &rect);
        }
    }
}

void ChessRenderer::render_result(SDL_Renderer* renderer, const TextureManager& textures,
                                 ResultType result, int width, int height) const {
    SDL_Texture* tex = textures.get_result_texture(result);
    if (!tex) return;

    // Center result banner over the board
    float banner_w = static_cast<float>(width) * 0.7f;
    float banner_h = static_cast<float>(height) * 0.35f;
    float x = (static_cast<float>(width) - banner_w) / 2.0f;
    float y = (static_cast<float>(height) - banner_h) / 2.0f;

    SDL_FRect rect{x, y, banner_w, banner_h};
    SDL_RenderTexture(renderer, tex, nullptr, &rect);
}

} // namespace chess::ui
