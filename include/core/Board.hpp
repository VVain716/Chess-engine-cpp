#pragma once

#include "core/Types.hpp"
#include "core/Move.hpp"
#include <array>
#include <vector>
#include <string>

namespace chess {

struct CastlingRights {
    bool white_kingside  = true;
    bool white_queenside = true;
    bool black_kingside  = true;
    bool black_queenside = true;
};

struct BoardState {
    CastlingRights castling;
    Square en_passant_square = SQ_NONE;
    Piece captured_piece = Piece::None;
    uint64_t hash = 0;
};

class Board {
private:
    std::array<Piece, 64> squares_{};
    Color side_to_move_ = Color::White;
    CastlingRights castling_{};
    Square en_passant_sq_ = SQ_NONE;
    Square white_king_sq_ = make_square(7, 4);
    Square black_king_sq_ = make_square(0, 4);
    uint64_t hash_ = 0;

    std::vector<BoardState> history_{};

public:
    Board();

    void reset_to_starting_position();
    void clear();

    [[nodiscard]] Piece get_piece(Square sq) const;
    [[nodiscard]] Piece get_piece(int row, int col) const;
    void set_piece(Square sq, Piece piece);
    void set_piece(int row, int col, Piece piece);

    [[nodiscard]] Color side_to_move() const { return side_to_move_; }
    void set_side_to_move(Color color) { side_to_move_ = color; }
    void switch_turn() { side_to_move_ = opponent_color(side_to_move_); }

    [[nodiscard]] Square king_square(Color color) const;
    [[nodiscard]] const CastlingRights& castling_rights() const { return castling_; }
    CastlingRights& castling_rights() { return castling_; }

    [[nodiscard]] Square en_passant_square() const { return en_passant_sq_; }
    void set_en_passant_square(Square sq) { en_passant_sq_ = sq; }

    [[nodiscard]] uint64_t hash() const { return hash_; }
    [[nodiscard]] uint64_t compute_hash() const;

    bool make_move(const Move& move);
    void undo_move(const Move& move);

    void make_null_move();
    void undo_null_move();

    bool load_fen(const std::string& fen);
    [[nodiscard]] std::string to_fen() const;

    // Helpers to find king square dynamically if needed
    void update_king_squares();
};

} // namespace chess
