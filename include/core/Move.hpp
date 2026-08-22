#pragma once

#include "core/Types.hpp"
#include <string>

namespace chess {

enum class MoveType : uint8_t {
    Normal,
    CastlingKingside,
    CastlingQueenside,
    EnPassant,
    Promotion
};

struct Move {
    Square from = SQ_NONE;
    Square to   = SQ_NONE;
    MoveType type = MoveType::Normal;
    PieceType promotion_piece = PieceType::None;

    constexpr Move() = default;
    constexpr Move(Square from_, Square to_, MoveType type_ = MoveType::Normal, PieceType promo = PieceType::None)
        : from(from_), to(to_), type(type_), promotion_piece(promo) {}

    constexpr bool operator==(const Move& other) const {
        return from == other.from && to == other.to && type == other.type && promotion_piece == other.promotion_piece;
    }

    constexpr bool operator!=(const Move& other) const {
        return !(*this == other);
    }

    constexpr bool is_castle() const {
        return type == MoveType::CastlingKingside || type == MoveType::CastlingQueenside;
    }

    constexpr bool is_valid() const {
        return from != SQ_NONE && to != SQ_NONE && from != to;
    }
};

} // namespace chess
