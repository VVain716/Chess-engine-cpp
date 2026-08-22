#include "ui/Animation.hpp"
#include <cmath>

namespace chess::ui {

void Animation::animate_move(SDL_Renderer* renderer, const Board& board, const Move& move,
                             const TextureManager& textures, const ChessRenderer& chess_renderer,
                             int width, int height, double duration_seconds) {
    Piece piece = board.get_piece(move.from);
    if (piece == Piece::None) return;

    SDL_Texture* piece_tex = textures.get_piece_texture(piece);
    if (!piece_tex) return;

    SDL_FRect start_rect = ChessRenderer::square_to_rect(move.from, width, height);
    SDL_FRect end_rect   = ChessRenderer::square_to_rect(move.to, width, height);

    // If castling, also prepare rook animation
    Piece rook_piece = Piece::None;
    SDL_Texture* rook_tex = nullptr;
    SDL_FRect rook_start_rect{}, rook_end_rect{};
    bool has_rook = false;

    if (move.type == MoveType::CastlingKingside) {
        has_rook = true;
        if (is_white(piece)) {
            rook_piece = Piece::WhiteRook;
            rook_start_rect = ChessRenderer::square_to_rect(make_square(7, 7), width, height);
            rook_end_rect   = ChessRenderer::square_to_rect(make_square(7, 5), width, height);
        } else {
            rook_piece = Piece::BlackRook;
            rook_start_rect = ChessRenderer::square_to_rect(make_square(0, 7), width, height);
            rook_end_rect   = ChessRenderer::square_to_rect(make_square(0, 5), width, height);
        }
        rook_tex = textures.get_piece_texture(rook_piece);
    } else if (move.type == MoveType::CastlingQueenside) {
        has_rook = true;
        if (is_white(piece)) {
            rook_piece = Piece::WhiteRook;
            rook_start_rect = ChessRenderer::square_to_rect(make_square(7, 0), width, height);
            rook_end_rect   = ChessRenderer::square_to_rect(make_square(7, 3), width, height);
        } else {
            rook_piece = Piece::BlackRook;
            rook_start_rect = ChessRenderer::square_to_rect(make_square(0, 0), width, height);
            rook_end_rect   = ChessRenderer::square_to_rect(make_square(0, 3), width, height);
        }
        rook_tex = textures.get_piece_texture(rook_piece);
    }

    uint64_t start_time = SDL_GetTicks();
    uint64_t total_ms = static_cast<uint64_t>(duration_seconds * 1000.0);
    if (total_ms == 0) total_ms = 1;

    while (true) {
        uint64_t elapsed = SDL_GetTicks() - start_time;
        float progress = static_cast<float>(elapsed) / static_cast<float>(total_ms);
        if (progress > 1.0f) progress = 1.0f;

        // Smooth ease-out quad
        float t = progress * (2.0f - progress);

        SDL_FRect curr_rect{
            start_rect.x + (end_rect.x - start_rect.x) * t,
            start_rect.y + (end_rect.y - start_rect.y) * t,
            start_rect.w,
            start_rect.h
        };

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        chess_renderer.draw_board(renderer, width, height, move.from, {move.to});
        chess_renderer.render_pieces(renderer, board, textures, width, height, move.from);

        SDL_RenderTexture(renderer, piece_tex, nullptr, &curr_rect);

        if (has_rook && rook_tex) {
            SDL_FRect curr_rook_rect{
                rook_start_rect.x + (rook_end_rect.x - rook_start_rect.x) * t,
                rook_start_rect.y + (rook_end_rect.y - rook_start_rect.y) * t,
                rook_start_rect.w,
                rook_start_rect.h
            };
            SDL_RenderTexture(renderer, rook_tex, nullptr, &curr_rook_rect);
        }

        SDL_RenderPresent(renderer);

        if (progress >= 1.0f) {
            break;
        }

        SDL_Delay(10); // ~100 FPS loop
    }
}

} // namespace chess::ui
