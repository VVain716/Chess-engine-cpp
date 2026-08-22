#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>

namespace chess {

enum class Color : uint8_t {
    White = 0,
    Black = 1,
    None  = 2
};

constexpr Color opponent_color(Color c) {
    return c == Color::White ? Color::Black : (c == Color::Black ? Color::White : Color::None);
}

enum class PieceType : uint8_t {
    None   = 0,
    Pawn   = 1,
    Knight = 2,
    Bishop = 3,
    Rook   = 4,
    Queen  = 5,
    King   = 6
};

enum class Piece : uint8_t {
    None        = 0,
    WhitePawn   = 1,
    WhiteKnight = 2,
    WhiteBishop = 3,
    WhiteRook   = 4,
    WhiteQueen  = 5,
    WhiteKing   = 6,
    BlackPawn   = 7,
    BlackKnight = 8,
    BlackBishop = 9,
    BlackRook   = 10,
    BlackQueen  = 11,
    BlackKing   = 12
};

constexpr Piece make_piece(Color color, PieceType type) {
    if (color == Color::None || type == PieceType::None) {
        return Piece::None;
    }
    uint8_t base = (color == Color::White) ? 0 : 6;
    return static_cast<Piece>(base + static_cast<uint8_t>(type));
}

constexpr Color piece_color(Piece piece) {
    auto val = static_cast<uint8_t>(piece);
    if (val >= 1 && val <= 6) return Color::White;
    if (val >= 7 && val <= 12) return Color::Black;
    return Color::None;
}

constexpr PieceType piece_type(Piece piece) {
    auto val = static_cast<uint8_t>(piece);
    if (val == 0) return PieceType::None;
    if (val <= 6) return static_cast<PieceType>(val);
    if (val <= 12) return static_cast<PieceType>(val - 6);
    return PieceType::None;
}

constexpr bool is_white(Piece piece) {
    return piece_color(piece) == Color::White;
}

constexpr bool is_black(Piece piece) {
    return piece_color(piece) == Color::Black;
}

using Square = int8_t;
constexpr Square SQ_NONE = -1;

constexpr Square make_square(int row, int col) {
    return static_cast<Square>(row * 8 + col);
}

constexpr int square_row(Square sq) {
    return sq / 8;
}

constexpr int square_col(Square sq) {
    return sq % 8;
}

constexpr bool is_valid_square(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

constexpr bool is_valid_square(Square sq) {
    return sq >= 0 && sq < 64;
}

std::string square_to_algebraic(Square sq);
std::optional<Square> square_from_algebraic(std::string_view str);
char piece_to_char(Piece piece);

} // namespace chess
