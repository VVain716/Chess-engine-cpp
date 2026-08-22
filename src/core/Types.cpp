#include "core/Types.hpp"

namespace chess {

std::string square_to_algebraic(Square sq) {
    if (!is_valid_square(sq)) return "-";
    int row = square_row(sq);
    int col = square_col(sq);
    char file = static_cast<char>('a' + col);
    char rank = static_cast<char>('8' - row);
    return std::string{file, rank};
}

std::optional<Square> square_from_algebraic(std::string_view str) {
    if (str.size() != 2) return std::nullopt;
    char file = str[0];
    char rank = str[1];
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        return std::nullopt;
    }
    int col = file - 'a';
    int row = '8' - rank;
    return make_square(row, col);
}

char piece_to_char(Piece piece) {
    PieceType type = piece_type(piece);
    char c = ' ';
    switch (type) {
        case PieceType::Pawn:   c = 'P'; break;
        case PieceType::Knight: c = 'N'; break;
        case PieceType::Bishop: c = 'B'; break;
        case PieceType::Rook:   c = 'R'; break;
        case PieceType::Queen:  c = 'Q'; break;
        case PieceType::King:   c = 'K'; break;
        default: break;
    }
    if (is_black(piece)) {
        c = static_cast<char>(std::tolower(c));
    }
    return c;
}

} // namespace chess
