#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "ui/TextureManager.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <optional>

namespace chess::ui {

struct ColorRGBA {
    uint8_t r, g, b, a;
};

struct Theme {
    ColorRGBA light_square  {241, 233, 232, 255};
    ColorRGBA dark_square   {153, 87, 81, 255};
    ColorRGBA highlight     {134, 151, 110, 220};
    ColorRGBA check_color   {226, 50, 34, 230};
    ColorRGBA last_move_col {190, 205, 130, 180};
};

class ChessRenderer {
private:
    Theme theme_{};

public:
    ChessRenderer() = default;

    void draw_board(SDL_Renderer* renderer, int width, int height,
                    std::optional<Square> selected_sq = std::nullopt,
                    const std::vector<Square>& highlighted_squares = {},
                    std::optional<Square> check_sq = std::nullopt) const;

    void render_pieces(SDL_Renderer* renderer, const Board& board,
                       const TextureManager& textures, int width, int height,
                       std::optional<Square> skip_sq = std::nullopt) const;

    void render_result(SDL_Renderer* renderer, const TextureManager& textures,
                       ResultType result, int width, int height) const;

    [[nodiscard]] static std::optional<Square> square_from_pixel(float x, float y, int width, int height);
    [[nodiscard]] static SDL_FRect square_to_rect(Square sq, int width, int height);
};

} // namespace chess::ui
