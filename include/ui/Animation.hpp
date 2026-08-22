#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "ui/ChessRenderer.hpp"
#include "ui/TextureManager.hpp"
#include <SDL3/SDL.h>

namespace chess::ui {

class Animation {
public:
    static void animate_move(SDL_Renderer* renderer, const Board& board, const Move& move,
                            const TextureManager& textures, const ChessRenderer& chess_renderer,
                            int width, int height, double duration_seconds = 0.15);
};

} // namespace chess::ui
