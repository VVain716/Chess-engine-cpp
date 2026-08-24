#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/Types.hpp"

namespace chess {

class Eval {
public:
    class Middlegame {
    public:
        // 2D 8x8 tables: [row][col] where row 0 is Rank 8 and row 7 is Rank 1
        static const int pawn_table[8][8];
        static const int knight_table[8][8];
        static const int bishop_table[8][8];
        static const int rook_table[8][8];
        static const int queen_table[8][8];
        static const int king_table[8][8];

        static int PawnValue(Square pos);
        static int KnightValue(Square pos);
        static int BishopValue(Square pos);
        static int RookValue(Square pos);
        static int QueenValue(Square pos);
        static int KingValue(Square pos);

        static int PawnValue(int row, int col);
        static int KnightValue(int row, int col);
        static int BishopValue(int row, int col);
        static int RookValue(int row, int col);
        static int QueenValue(int row, int col);
        static int KingValue(int row, int col);
    };

    class Endgame {
    public:
        // 2D 8x8 tables: [row][col] where row 0 is Rank 8 and row 7 is Rank 1
        static const int pawn_table[8][8];
        static const int knight_table[8][8];
        static const int bishop_table[8][8];
        static const int rook_table[8][8];
        static const int queen_table[8][8];
        static const int king_table[8][8];

        static int PawnValue(Square pos);
        static int KnightValue(Square pos);
        static int BishopValue(Square pos);
        static int RookValue(Square pos);
        static int QueenValue(Square pos);
        static int KingValue(Square pos);

        static int PawnValue(int row, int col);
        static int KnightValue(int row, int col);
        static int BishopValue(int row, int col);
        static int RookValue(int row, int col);
        static int QueenValue(int row, int col);
        static int KingValue(int row, int col);
    };

    static int get_game_phase(const Board &board);
    static int white_eval(const Board &board);
    static int black_eval(const Board &board);
    static int evaluate(const Board &board);

    // Positional / Structural evaluation terms
    static std::pair<int, int> evaluate_pawns(const Board &board, Color color);
    static std::pair<int, int> evaluate_rooks(const Board &board, Color color);
    static std::pair<int, int> evaluate_bishops(const Board &board, Color color, int total_pawns);
    static std::pair<int, int> evaluate_knights(const Board &board, Color color, int total_pawns);
    static std::pair<int, int> evaluate_king(const Board &board, Color color);
};

} // namespace chess

