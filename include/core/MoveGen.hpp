#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include <vector>
#include <optional>

namespace chess {

class MoveGen {
public:
    static bool is_square_attacked(const Board& board, Square sq, Color attacker_color);
    static bool is_in_check(const Board& board, Color color);

    static std::vector<Move> generate_pseudo_legal_moves(const Board& board, std::optional<Square> from_sq = std::nullopt);
    static std::vector<Move> generate_legal_moves(Board& board, std::optional<Square> from_sq = std::nullopt);

    // Convenience API for getting valid legal moves
    // 1. For all pieces in the current board position
    static std::vector<Move> get_legal_moves(Board& board);
    // 2. For a specific piece at a given square position (0..63)
    static std::vector<Move> get_legal_moves(Board& board, Square sq);
    // 3. For a specific piece at (row, col) coordinates
    static std::vector<Move> get_legal_moves(Board& board, int row, int col);

    static bool is_checkmate(Board& board);
    static bool is_stalemate(Board& board);
    static bool is_insufficient_material(const Board& board);
    static bool is_draw(Board& board);
};

} // namespace chess
