#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/MoveGen.hpp"
#include "core/Eval.hpp"
#include "core/TranspositionTable.hpp"
#include <limits>
#include <vector>
#include <chrono>

namespace chess {

constexpr int INFINITY_SCORE = 1000000;
constexpr int MATE_SCORE     = 100000;
constexpr int MAX_PLY        = 64;

struct SearchResult {
    Move best_move;
    int score = 0;
    int nodes = 0;
    int depth = 0;
    int seldepth = 0;
    int time_ms = 0;
    uint64_t nps = 0;
    int hashfull = 0;
    std::vector<Move> pv;
};

class Search {
private:
    static inline TranspositionTable tt_{16};
    static inline Move killer_moves_[MAX_PLY][2]{};
    static inline int history_table_[2][64][64]{};

    static bool check_time(int nodes, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms);
    static int quiescence(Board& board, int alpha, int beta, int& nodes, int ply, int& seldepth, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped);
    static int negamax(Board& board, int depth, int ply, int alpha, int beta, int& nodes, int& seldepth, Move& best_move_root, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped, bool allow_null = true);
    static void order_moves(std::vector<Move>& moves, const Board& board, const Move& tt_move = Move{}, int ply = 0);
    static int score_move(const Move& move, const Board& board, const Move& tt_move, int ply);
    static bool has_non_pawn_material(const Board& board, Color color);

public:
    static TranspositionTable& get_tt() { return tt_; }
    static void clear_tt() { tt_.clear(); }
    static void resize_tt(size_t size_mb) { tt_.resize(size_mb); }

    static std::vector<Move> get_pv_line(Board& board, int max_depth);

    static SearchResult search(Board& board, int depth = 64, int time_limit_ms = -1, bool output_uci_info = false);
    static Move get_best_move(Board& board, int depth = 4, int time_limit_ms = -1);
    static int minmax_eval(Board &board, int depth = 4);
};

} // namespace chess
