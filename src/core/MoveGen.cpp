#include "core/MoveGen.hpp"
#include <array>

namespace chess {

namespace {

constexpr std::array<std::pair<int, int>, 8> KNIGHT_OFFSETS = {{
    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
    { 1, -2}, { 1, 2}, { 2, -1}, { 2, 1}
}};

constexpr std::array<std::pair<int, int>, 8> KING_OFFSETS = {{
    {-1, -1}, {-1,  0}, {-1,  1},
    { 0, -1},           { 0,  1},
    { 1, -1}, { 1,  0}, { 1,  1}
}};

constexpr std::array<std::pair<int, int>, 4> BISHOP_DIRS = {{
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
}};

constexpr std::array<std::pair<int, int>, 4> ROOK_DIRS = {{
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
}};

} // namespace

bool MoveGen::is_square_attacked(const Board& board, Square sq, Color attacker_color) {
    if (!is_valid_square(sq)) return false;
    int sq_row = square_row(sq);
    int sq_col = square_col(sq);

    // 1. Pawn attacks
    if (attacker_color == Color::White) {
        // White pawns attack upwards (from sq_row + 1)
        int p_row = sq_row + 1;
        if (p_row < 8) {
            if (sq_col - 1 >= 0 && board.get_piece(p_row, sq_col - 1) == Piece::WhitePawn) return true;
            if (sq_col + 1 < 8 && board.get_piece(p_row, sq_col + 1) == Piece::WhitePawn) return true;
        }
    } else {
        // Black pawns attack downwards (from sq_row - 1)
        int p_row = sq_row - 1;
        if (p_row >= 0) {
            if (sq_col - 1 >= 0 && board.get_piece(p_row, sq_col - 1) == Piece::BlackPawn) return true;
            if (sq_col + 1 < 8 && board.get_piece(p_row, sq_col + 1) == Piece::BlackPawn) return true;
        }
    }

    // 2. Knight attacks
    Piece attacker_knight = make_piece(attacker_color, PieceType::Knight);
    for (const auto& [dr, dc] : KNIGHT_OFFSETS) {
        int r = sq_row + dr;
        int c = sq_col + dc;
        if (is_valid_square(r, c) && board.get_piece(r, c) == attacker_knight) {
            return true;
        }
    }

    // 3. King attacks
    Piece attacker_king = make_piece(attacker_color, PieceType::King);
    for (const auto& [dr, dc] : KING_OFFSETS) {
        int r = sq_row + dr;
        int c = sq_col + dc;
        if (is_valid_square(r, c) && board.get_piece(r, c) == attacker_king) {
            return true;
        }
    }

    // 4. Bishop / Queen attacks (diagonal rays)
    Piece attacker_bishop = make_piece(attacker_color, PieceType::Bishop);
    Piece attacker_queen  = make_piece(attacker_color, PieceType::Queen);
    for (const auto& [dr, dc] : BISHOP_DIRS) {
        int r = sq_row + dr;
        int c = sq_col + dc;
        while (is_valid_square(r, c)) {
            Piece p = board.get_piece(r, c);
            if (p != Piece::None) {
                if (p == attacker_bishop || p == attacker_queen) return true;
                break; // blocked
            }
            r += dr;
            c += dc;
        }
    }

    // 5. Rook / Queen attacks (straight rays)
    Piece attacker_rook = make_piece(attacker_color, PieceType::Rook);
    for (const auto& [dr, dc] : ROOK_DIRS) {
        int r = sq_row + dr;
        int c = sq_col + dc;
        while (is_valid_square(r, c)) {
            Piece p = board.get_piece(r, c);
            if (p != Piece::None) {
                if (p == attacker_rook || p == attacker_queen) return true;
                break; // blocked
            }
            r += dr;
            c += dc;
        }
    }

    return false;
}

bool MoveGen::is_in_check(const Board& board, Color color) {
    Square ksq = board.king_square(color);
    if (!is_valid_square(ksq)) return false;
    return is_square_attacked(board, ksq, opponent_color(color));
}

std::vector<Move> MoveGen::generate_pseudo_legal_moves(const Board& board, std::optional<Square> from_sq) {
    std::vector<Move> moves;
    Color side = board.side_to_move();

    auto add_piece_moves = [&](Square sq, Piece piece) {
        if (piece_color(piece) != side) return;
        PieceType type = piece_type(piece);
        int r = square_row(sq);
        int c = square_col(sq);

        if (type == PieceType::Pawn) {
            int forward_dir = (side == Color::White) ? -1 : 1;
            int start_row   = (side == Color::White) ? 6 : 1;
            int promo_row   = (side == Color::White) ? 0 : 7;

            // Single push
            int next_r = r + forward_dir;
            if (is_valid_square(next_r, c) && board.get_piece(next_r, c) == Piece::None) {
                Square dest = make_square(next_r, c);
                if (next_r == promo_row) {
                    moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Queen);
                    moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Rook);
                    moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Bishop);
                    moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Knight);
                } else {
                    moves.emplace_back(sq, dest, MoveType::Normal);
                    // Double push
                    if (r == start_row) {
                        int double_r = r + 2 * forward_dir;
                        if (is_valid_square(double_r, c) && board.get_piece(double_r, c) == Piece::None) {
                            moves.emplace_back(sq, make_square(double_r, c), MoveType::Normal);
                        }
                    }
                }
            }

            // Captures
            for (int dc : {-1, 1}) {
                int cap_c = c + dc;
                if (is_valid_square(next_r, cap_c)) {
                    Square dest = make_square(next_r, cap_c);
                    Piece target = board.get_piece(dest);
                    if (target != Piece::None && piece_color(target) == opponent_color(side)) {
                        if (next_r == promo_row) {
                            moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Queen);
                            moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Rook);
                            moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Bishop);
                            moves.emplace_back(sq, dest, MoveType::Promotion, PieceType::Knight);
                        } else {
                            moves.emplace_back(sq, dest, MoveType::Normal);
                        }
                    } else if (dest == board.en_passant_square()) {
                        moves.emplace_back(sq, dest, MoveType::EnPassant);
                    }
                }
            }
        } else if (type == PieceType::Knight) {
            for (const auto& [dr, dc] : KNIGHT_OFFSETS) {
                int nr = r + dr;
                int nc = c + dc;
                if (is_valid_square(nr, nc)) {
                    Piece target = board.get_piece(nr, nc);
                    if (target == Piece::None || piece_color(target) == opponent_color(side)) {
                        moves.emplace_back(sq, make_square(nr, nc), MoveType::Normal);
                    }
                }
            }
        } else if (type == PieceType::King) {
            for (const auto& [dr, dc] : KING_OFFSETS) {
                int nr = r + dr;
                int nc = c + dc;
                if (is_valid_square(nr, nc)) {
                    Piece target = board.get_piece(nr, nc);
                    if (target == Piece::None || piece_color(target) == opponent_color(side)) {
                        moves.emplace_back(sq, make_square(nr, nc), MoveType::Normal);
                    }
                }
            }

            // Castling
            const auto& castling = board.castling_rights();
            Color opp = opponent_color(side);
            if (side == Color::White) {
                // White Kingside: e1(7,4) -> g1(7,6)
                if (castling.white_kingside &&
                    board.get_piece(7, 4) == Piece::WhiteKing &&
                    board.get_piece(7, 7) == Piece::WhiteRook &&
                    board.get_piece(7, 5) == Piece::None &&
                    board.get_piece(7, 6) == Piece::None &&
                    !is_square_attacked(board, make_square(7, 4), opp) &&
                    !is_square_attacked(board, make_square(7, 5), opp) &&
                    !is_square_attacked(board, make_square(7, 6), opp)) {
                    moves.emplace_back(make_square(7, 4), make_square(7, 6), MoveType::CastlingKingside);
                }
                // White Queenside: e1(7,4) -> c1(7,2)
                if (castling.white_queenside &&
                    board.get_piece(7, 4) == Piece::WhiteKing &&
                    board.get_piece(7, 0) == Piece::WhiteRook &&
                    board.get_piece(7, 1) == Piece::None &&
                    board.get_piece(7, 2) == Piece::None &&
                    board.get_piece(7, 3) == Piece::None &&
                    !is_square_attacked(board, make_square(7, 4), opp) &&
                    !is_square_attacked(board, make_square(7, 3), opp) &&
                    !is_square_attacked(board, make_square(7, 2), opp)) {
                    moves.emplace_back(make_square(7, 4), make_square(7, 2), MoveType::CastlingQueenside);
                }
            } else {
                // Black Kingside: e8(0,4) -> g8(0,6)
                if (castling.black_kingside &&
                    board.get_piece(0, 4) == Piece::BlackKing &&
                    board.get_piece(0, 7) == Piece::BlackRook &&
                    board.get_piece(0, 5) == Piece::None &&
                    board.get_piece(0, 6) == Piece::None &&
                    !is_square_attacked(board, make_square(0, 4), opp) &&
                    !is_square_attacked(board, make_square(0, 5), opp) &&
                    !is_square_attacked(board, make_square(0, 6), opp)) {
                    moves.emplace_back(make_square(0, 4), make_square(0, 6), MoveType::CastlingKingside);
                }
                // Black Queenside: e8(0,4) -> c8(0,2)
                if (castling.black_queenside &&
                    board.get_piece(0, 4) == Piece::BlackKing &&
                    board.get_piece(0, 0) == Piece::BlackRook &&
                    board.get_piece(0, 1) == Piece::None &&
                    board.get_piece(0, 2) == Piece::None &&
                    board.get_piece(0, 3) == Piece::None &&
                    !is_square_attacked(board, make_square(0, 4), opp) &&
                    !is_square_attacked(board, make_square(0, 3), opp) &&
                    !is_square_attacked(board, make_square(0, 2), opp)) {
                    moves.emplace_back(make_square(0, 4), make_square(0, 2), MoveType::CastlingQueenside);
                }
            }
        } else {
            // Sliding pieces: Bishop, Rook, Queen
            bool is_diag = (type == PieceType::Bishop || type == PieceType::Queen);
            bool is_straight = (type == PieceType::Rook || type == PieceType::Queen);

            auto slide = [&](const auto& dirs) {
                for (const auto& [dr, dc] : dirs) {
                    int nr = r + dr;
                    int nc = c + dc;
                    while (is_valid_square(nr, nc)) {
                        Piece target = board.get_piece(nr, nc);
                        if (target == Piece::None) {
                            moves.emplace_back(sq, make_square(nr, nc), MoveType::Normal);
                        } else {
                            if (piece_color(target) == opponent_color(side)) {
                                moves.emplace_back(sq, make_square(nr, nc), MoveType::Normal);
                            }
                            break; // blocked
                        }
                        nr += dr;
                        nc += dc;
                    }
                }
            };

            if (is_diag) slide(BISHOP_DIRS);
            if (is_straight) slide(ROOK_DIRS);
        }
    };

    if (from_sq.has_value()) {
        if (is_valid_square(*from_sq)) {
            add_piece_moves(*from_sq, board.get_piece(*from_sq));
        }
    } else {
        for (Square sq = 0; sq < 64; ++sq) {
            add_piece_moves(sq, board.get_piece(sq));
        }
    }

    return moves;
}

std::vector<Move> MoveGen::generate_legal_moves(Board& board, std::optional<Square> from_sq) {
    std::vector<Move> pseudo = generate_pseudo_legal_moves(board, from_sq);
    std::vector<Move> legal;
    legal.reserve(pseudo.size());

    Color moving_side = board.side_to_move();

    for (const auto& move : pseudo) {
        board.make_move(move);
        if (!is_in_check(board, moving_side)) {
            legal.push_back(move);
        }
        board.undo_move(move);
    }

    return legal;
}

std::vector<Move> MoveGen::get_legal_moves(Board& board) {
    return generate_legal_moves(board, std::nullopt);
}

std::vector<Move> MoveGen::get_legal_moves(Board& board, Square sq) {
    if (!is_valid_square(sq)) return {};
    return generate_legal_moves(board, sq);
}

std::vector<Move> MoveGen::get_legal_moves(Board& board, int row, int col) {
    if (!is_valid_square(row, col)) return {};
    return generate_legal_moves(board, make_square(row, col));
}

bool MoveGen::is_checkmate(Board& board) {
    if (!is_in_check(board, board.side_to_move())) {
        return false;
    }
    return generate_legal_moves(board).empty();
}

bool MoveGen::is_stalemate(Board& board) {
    if (is_in_check(board, board.side_to_move())) {
        return false;
    }
    return generate_legal_moves(board).empty();
}

} // namespace chess
