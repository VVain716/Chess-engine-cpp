#include "core/Notation.hpp"

namespace chess {

namespace {
constexpr const char* ESCAPE = "\033[0m";
constexpr const char* RED    = "\033[91;1m";
constexpr const char* GREEN  = "\033[92;1m";
constexpr const char* YELLOW = "\033[93;1m";
} // namespace

void MoveHistory::clear() {
    history_str_.clear();
    move_number_ = 1;
}

std::string MoveHistory::format_move(const Board& board_before_move, const Move& move) {
    if (move.type == MoveType::CastlingKingside) {
        return "0-0";
    }
    if (move.type == MoveType::CastlingQueenside) {
        return "0-0-0";
    }

    Piece moving_piece = board_before_move.get_piece(move.from);
    PieceType type = piece_type(moving_piece);

    std::string piece_prefix;
    switch (type) {
        case PieceType::Knight: piece_prefix = "N"; break;
        case PieceType::Bishop: piece_prefix = "B"; break;
        case PieceType::Rook:   piece_prefix = "R"; break;
        case PieceType::Queen:  piece_prefix = "Q"; break;
        case PieceType::King:   piece_prefix = "K"; break;
        default: break;
    }

    std::string from_str = square_to_algebraic(move.from);
    std::string to_str = square_to_algebraic(move.to);

    Piece target = board_before_move.get_piece(move.to);
    char sep = (target != Piece::None || move.type == MoveType::EnPassant) ? 'x' : '-';

    std::string result = piece_prefix + from_str + sep + to_str;

    if (move.type == MoveType::Promotion) {
        char promo_char = '=';
        switch (move.promotion_piece) {
            case PieceType::Queen:  promo_char = 'Q'; break;
            case PieceType::Rook:   promo_char = 'R'; break;
            case PieceType::Bishop: promo_char = 'B'; break;
            case PieceType::Knight: promo_char = 'N'; break;
            default: break;
        }
        result += std::string{"="} + promo_char;
    }

    return result;
}

void MoveHistory::record_move(const Board& board_before_move, const Move& move) {
    std::string formatted = format_move(board_before_move, move);
    if (board_before_move.side_to_move() == Color::White) {
        history_str_ += std::to_string(move_number_) + "." + formatted + " ";
    } else {
        history_str_ += formatted + " ";
        move_number_++;
    }
}

void MoveHistory::record_white_win() {
    history_str_ += std::string("\n") + GREEN + "1-0" + ESCAPE;
}

void MoveHistory::record_black_win() {
    history_str_ += std::string("\n") + RED + "0-1" + ESCAPE;
}

void MoveHistory::record_draw() {
    history_str_ += std::string("\n") + YELLOW + "0.5-0.5" + ESCAPE;
}

} // namespace chess
