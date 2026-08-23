#include "core/Board.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace chess {

Board::Board() {
    reset_to_starting_position();
}

void Board::clear() {
    squares_.fill(Piece::None);
    side_to_move_ = Color::White;
    castling_ = CastlingRights{false, false, false, false};
    en_passant_sq_ = SQ_NONE;
    white_king_sq_ = SQ_NONE;
    black_king_sq_ = SQ_NONE;
    history_.clear();
}

void Board::reset_to_starting_position() {
    squares_.fill(Piece::None);

    // Black pieces (row 0 & 1)
    set_piece(0, 0, Piece::BlackRook);
    set_piece(0, 1, Piece::BlackKnight);
    set_piece(0, 2, Piece::BlackBishop);
    set_piece(0, 3, Piece::BlackQueen);
    set_piece(0, 4, Piece::BlackKing);
    set_piece(0, 5, Piece::BlackBishop);
    set_piece(0, 6, Piece::BlackKnight);
    set_piece(0, 7, Piece::BlackRook);
    for (int col = 0; col < 8; ++col) {
        set_piece(1, col, Piece::BlackPawn);
    }

    // White pieces (row 6 & 7)
    for (int col = 0; col < 8; ++col) {
        set_piece(6, col, Piece::WhitePawn);
    }
    set_piece(7, 0, Piece::WhiteRook);
    set_piece(7, 1, Piece::WhiteKnight);
    set_piece(7, 2, Piece::WhiteBishop);
    set_piece(7, 3, Piece::WhiteQueen);
    set_piece(7, 4, Piece::WhiteKing);
    set_piece(7, 5, Piece::WhiteBishop);
    set_piece(7, 6, Piece::WhiteKnight);
    set_piece(7, 7, Piece::WhiteRook);

    side_to_move_ = Color::White;
    castling_ = CastlingRights{true, true, true, true};
    en_passant_sq_ = SQ_NONE;
    white_king_sq_ = make_square(7, 4);
    black_king_sq_ = make_square(0, 4);
    history_.clear();
}

Piece Board::get_piece(Square sq) const {
    if (!is_valid_square(sq)) return Piece::None;
    return squares_[static_cast<size_t>(sq)];
}

Piece Board::get_piece(int row, int col) const {
    if (!is_valid_square(row, col)) return Piece::None;
    return squares_[static_cast<size_t>(make_square(row, col))];
}

void Board::set_piece(Square sq, Piece piece) {
    if (!is_valid_square(sq)) return;
    squares_[static_cast<size_t>(sq)] = piece;
    if (piece == Piece::WhiteKing) white_king_sq_ = sq;
    else if (piece == Piece::BlackKing) black_king_sq_ = sq;
}

void Board::set_piece(int row, int col, Piece piece) {
    if (!is_valid_square(row, col)) return;
    set_piece(make_square(row, col), piece);
}

Square Board::king_square(Color color) const {
    return (color == Color::White) ? white_king_sq_ : black_king_sq_;
}

void Board::update_king_squares() {
    white_king_sq_ = SQ_NONE;
    black_king_sq_ = SQ_NONE;
    for (Square sq = 0; sq < 64; ++sq) {
        if (squares_[sq] == Piece::WhiteKing) white_king_sq_ = sq;
        else if (squares_[sq] == Piece::BlackKing) black_king_sq_ = sq;
    }
}

bool Board::make_move(const Move& move) {
    if (!move.is_valid()) return false;

    Piece moving_piece = get_piece(move.from);
    if (moving_piece == Piece::None) return false;

    Piece captured_piece = get_piece(move.to);

    // Record state for undo
    BoardState state;
    state.castling = castling_;
    state.en_passant_square = en_passant_sq_;
    state.captured_piece = captured_piece;
    history_.push_back(state);

    // Reset en passant
    en_passant_sq_ = SQ_NONE;

    // Handle move types
    if (move.type == MoveType::CastlingKingside) {
        if (is_white(moving_piece)) {
            // White kingside castle: King e1 (60) -> g1 (62), Rook h1 (63) -> f1 (61)
            set_piece(make_square(7, 4), Piece::None);
            set_piece(make_square(7, 6), Piece::WhiteKing);
            set_piece(make_square(7, 7), Piece::None);
            set_piece(make_square(7, 5), Piece::WhiteRook);
        } else {
            // Black kingside castle: King e8 (4) -> g8 (6), Rook h8 (7) -> f8 (5)
            set_piece(make_square(0, 4), Piece::None);
            set_piece(make_square(0, 6), Piece::BlackKing);
            set_piece(make_square(0, 7), Piece::None);
            set_piece(make_square(0, 5), Piece::BlackRook);
        }
    } else if (move.type == MoveType::CastlingQueenside) {
        if (is_white(moving_piece)) {
            // White queenside castle: King e1 (60) -> c1 (58), Rook a1 (56) -> d1 (59)
            set_piece(make_square(7, 4), Piece::None);
            set_piece(make_square(7, 2), Piece::WhiteKing);
            set_piece(make_square(7, 0), Piece::None);
            set_piece(make_square(7, 3), Piece::WhiteRook);
        } else {
            // Black queenside castle: King e8 (4) -> c8 (2), Rook a8 (0) -> d8 (3)
            set_piece(make_square(0, 4), Piece::None);
            set_piece(make_square(0, 2), Piece::BlackKing);
            set_piece(make_square(0, 0), Piece::None);
            set_piece(make_square(0, 3), Piece::BlackRook);
        }
    } else if (move.type == MoveType::EnPassant) {
        set_piece(move.from, Piece::None);
        set_piece(move.to, moving_piece);
        // Remove captured pawn
        int captured_row = square_row(move.from);
        int captured_col = square_col(move.to);
        set_piece(make_square(captured_row, captured_col), Piece::None);
    } else if (move.type == MoveType::Promotion) {
        set_piece(move.from, Piece::None);
        Piece promo = make_piece(piece_color(moving_piece), move.promotion_piece);
        set_piece(move.to, promo);
    } else {
        // Normal move
        set_piece(move.from, Piece::None);
        set_piece(move.to, moving_piece);

        // Check if double pawn push sets en passant square
        if (piece_type(moving_piece) == PieceType::Pawn) {
            int from_row = square_row(move.from);
            int to_row = square_row(move.to);
            if (std::abs(to_row - from_row) == 2) {
                en_passant_sq_ = make_square((from_row + to_row) / 2, square_col(move.from));
            }
        }
    }

    // Update castling rights if king or rook moved or rook was captured
    if (moving_piece == Piece::WhiteKing) {
        castling_.white_kingside = false;
        castling_.white_queenside = false;
    } else if (moving_piece == Piece::BlackKing) {
        castling_.black_kingside = false;
        castling_.black_queenside = false;
    }

    if (move.from == make_square(7, 0) || move.to == make_square(7, 0)) castling_.white_queenside = false;
    if (move.from == make_square(7, 7) || move.to == make_square(7, 7)) castling_.white_kingside = false;
    if (move.from == make_square(0, 0) || move.to == make_square(0, 0)) castling_.black_queenside = false;
    if (move.from == make_square(0, 7) || move.to == make_square(0, 7)) castling_.black_kingside = false;

    switch_turn();
    return true;
}

void Board::undo_move(const Move& move) {
    if (history_.empty() || !move.is_valid()) return;

    BoardState prev_state = history_.back();
    history_.pop_back();

    castling_ = prev_state.castling;
    en_passant_sq_ = prev_state.en_passant_square;
    switch_turn();

    Piece moving_piece = get_piece(move.to);

    if (move.type == MoveType::CastlingKingside) {
        if (side_to_move_ == Color::White) {
            set_piece(make_square(7, 4), Piece::WhiteKing);
            set_piece(make_square(7, 6), Piece::None);
            set_piece(make_square(7, 7), Piece::WhiteRook);
            set_piece(make_square(7, 5), Piece::None);
        } else {
            set_piece(make_square(0, 4), Piece::BlackKing);
            set_piece(make_square(0, 6), Piece::None);
            set_piece(make_square(0, 7), Piece::BlackRook);
            set_piece(make_square(0, 5), Piece::None);
        }
    } else if (move.type == MoveType::CastlingQueenside) {
        if (side_to_move_ == Color::White) {
            set_piece(make_square(7, 4), Piece::WhiteKing);
            set_piece(make_square(7, 2), Piece::None);
            set_piece(make_square(7, 0), Piece::WhiteRook);
            set_piece(make_square(7, 3), Piece::None);
        } else {
            set_piece(make_square(0, 4), Piece::BlackKing);
            set_piece(make_square(0, 2), Piece::None);
            set_piece(make_square(0, 0), Piece::BlackRook);
            set_piece(make_square(0, 3), Piece::None);
        }
    } else if (move.type == MoveType::EnPassant) {
        set_piece(move.from, moving_piece);
        set_piece(move.to, Piece::None);
        int captured_row = square_row(move.from);
        int captured_col = square_col(move.to);
        Piece enemy_pawn = (side_to_move_ == Color::White) ? Piece::BlackPawn : Piece::WhitePawn;
        set_piece(make_square(captured_row, captured_col), enemy_pawn);
    } else if (move.type == MoveType::Promotion) {
        Piece pawn = make_piece(side_to_move_, PieceType::Pawn);
        set_piece(move.from, pawn);
        set_piece(move.to, prev_state.captured_piece);
    } else {
        set_piece(move.from, moving_piece);
        set_piece(move.to, prev_state.captured_piece);
    }

    update_king_squares();
}

bool Board::load_fen(const std::string& fen) {
    std::istringstream iss(fen);
    std::string pieces_str, active_color, castling, ep_square;
    
    if (!(iss >> pieces_str)) {
        return false;
    }

    clear();

    // 1. Piece placement
    int row = 0;
    int col = 0;
    for (char c : pieces_str) {
        if (c == '/') {
            row++;
            col = 0;
            if (row > 7) break;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            col += (c - '0');
        } else {
            Piece piece = Piece::None;
            switch (c) {
                case 'P': piece = Piece::WhitePawn; break;
                case 'N': piece = Piece::WhiteKnight; break;
                case 'B': piece = Piece::WhiteBishop; break;
                case 'R': piece = Piece::WhiteRook; break;
                case 'Q': piece = Piece::WhiteQueen; break;
                case 'K': piece = Piece::WhiteKing; break;
                case 'p': piece = Piece::BlackPawn; break;
                case 'n': piece = Piece::BlackKnight; break;
                case 'b': piece = Piece::BlackBishop; break;
                case 'r': piece = Piece::BlackRook; break;
                case 'q': piece = Piece::BlackQueen; break;
                case 'k': piece = Piece::BlackKing; break;
                default: break;
            }
            if (piece != Piece::None && is_valid_square(row, col)) {
                set_piece(row, col, piece);
            }
            col++;
        }
    }

    // 2. Active color
    if (iss >> active_color) {
        side_to_move_ = (active_color == "b" || active_color == "B") ? Color::Black : Color::White;
    } else {
        side_to_move_ = Color::White;
    }

    // 3. Castling rights
    castling_ = CastlingRights{false, false, false, false};
    if (iss >> castling) {
        if (castling != "-") {
            for (char c : castling) {
                if (c == 'K') castling_.white_kingside = true;
                else if (c == 'Q') castling_.white_queenside = true;
                else if (c == 'k') castling_.black_kingside = true;
                else if (c == 'q') castling_.black_queenside = true;
            }
        }
    }

    // 4. En passant square
    en_passant_sq_ = SQ_NONE;
    if (iss >> ep_square) {
        if (ep_square != "-") {
            auto sq = square_from_algebraic(ep_square);
            if (sq.has_value()) {
                en_passant_sq_ = *sq;
            }
        }
    }

    update_king_squares();
    history_.clear();
    return true;
}

std::string Board::to_fen() const {
    std::string fen;
    // 1. Piece placement
    for (int r = 0; r < 8; ++r) {
        int empty_count = 0;
        for (int c = 0; c < 8; ++c) {
            Piece p = get_piece(r, c);
            if (p == Piece::None) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen += std::to_string(empty_count);
                    empty_count = 0;
                }
                fen += piece_to_char(p);
            }
        }
        if (empty_count > 0) {
            fen += std::to_string(empty_count);
        }
        if (r < 7) {
            fen += '/';
        }
    }

    // 2. Side to move
    fen += (side_to_move_ == Color::White) ? " w " : " b ";

    // 3. Castling rights
    std::string castling_str;
    if (castling_.white_kingside) castling_str += 'K';
    if (castling_.white_queenside) castling_str += 'Q';
    if (castling_.black_kingside) castling_str += 'k';
    if (castling_.black_queenside) castling_str += 'q';
    if (castling_str.empty()) castling_str = "-";
    fen += castling_str + " ";

    // 4. En passant square
    if (en_passant_sq_ != SQ_NONE) {
        fen += square_to_algebraic(en_passant_sq_);
    } else {
        fen += "-";
    }

    // 5. Halfmove clock and fullmove number
    fen += " 0 1";

    return fen;
}

} // namespace chess
