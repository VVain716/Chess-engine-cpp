#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/MoveGen.hpp"
#include "core/Eval.hpp"
#include <limits>
#include <vector>

#include <chrono>

namespace chess {

constexpr int INFINITY_SCORE = 1000000;
constexpr int MATE_SCORE     = 100000;

struct SearchResult {
    Move best_move;
    int score = 0;
    int nodes = 0;
    int depth = 0;
};

class Search {
private:
    static bool check_time(int nodes, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms);
    static int quiescence(Board& board, int alpha, int beta, int& nodes, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped);
    static int negamax(Board& board, int depth, int ply, int alpha, int beta, int& nodes, Move& best_move_root, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped);
    static void order_moves(std::vector<Move>& moves, const Board& board);
    static int score_move(const Move& move, const Board& board);

public:
    static SearchResult search(Board& board, int depth = 64, int time_limit_ms = -1);
    static Move get_best_move(Board& board, int depth = 4, int time_limit_ms = -1);
    static int minmax_eval(Board &board, int depth = 4);
};

} // namespace chess
