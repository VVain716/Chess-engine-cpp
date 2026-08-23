#include "core/Search.hpp"
#include <algorithm>

namespace chess {

namespace {

int get_piece_value(PieceType type) {
    switch (type) {
        case PieceType::Pawn:   return 100;
        case PieceType::Knight: return 320;
        case PieceType::Bishop: return 330;
        case PieceType::Rook:   return 500;
        case PieceType::Queen:  return 900;
        case PieceType::King:   return 20000;
        default: return 0;
    }
}

int get_side_eval(const Board& board) {
    int eval = Eval::evaluate(board); // eval is white - black
    return (board.side_to_move() == Color::White) ? eval : -eval;
}

} // namespace

int Search::score_move(const Move& move, const Board& board) {
    Piece moving_piece = board.get_piece(move.from);
    Piece target_piece = board.get_piece(move.to);

    // 1. Captures (MVV-LVA: Most Valuable Victim - Least Valuable Attacker)
    if (target_piece != Piece::None) {
        int victim_val   = get_piece_value(piece_type(target_piece));
        int attacker_val = get_piece_value(piece_type(moving_piece));
        return 10000 + (victim_val * 10 - attacker_val);
    }

    if (move.type == MoveType::EnPassant) {
        return 10000 + (100 * 10 - 100); // Pawn takes pawn
    }

    // 2. Promotions
    if (move.type == MoveType::Promotion) {
        return 9000 + get_piece_value(move.promotion_piece);
    }

    // 3. Castling
    if (move.is_castle()) {
        return 500;
    }

    return 0;
}

void Search::order_moves(std::vector<Move>& moves, const Board& board) {
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return score_move(a, board) > score_move(b, board);
    });
}

bool Search::check_time(int nodes, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms) {
    if (time_limit_ms <= 0) return false;
    // Check every 1024 nodes to balance responsiveness and clock query overhead
    if ((nodes & 1023) == 0) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        if (elapsed >= time_limit_ms) {
            return true;
        }
    }
    return false;
}

int Search::quiescence(Board& board, int alpha, int beta, int& nodes, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped) {
    ++nodes;

    if (check_time(nodes, start_time, time_limit_ms)) {
        stopped = true;
        return alpha;
    }

    bool in_check = MoveGen::is_in_check(board, board.side_to_move());

    if (!in_check) {
        int stand_pat = get_side_eval(board);
        if (stand_pat >= beta) {
            return beta;
        }
        if (stand_pat > alpha) {
            alpha = stand_pat;
        }
    }

    // Generate pseudo moves and filter only captures (or all moves if in check)
    auto moves = MoveGen::generate_legal_moves(board);

    std::vector<Move> tactical_moves;
    tactical_moves.reserve(moves.size());

    for (const auto& m : moves) {
        if (in_check || board.get_piece(m.to) != Piece::None || m.type == MoveType::EnPassant || m.type == MoveType::Promotion) {
            tactical_moves.push_back(m);
        }
    }

    if (in_check && moves.empty()) {
        return -MATE_SCORE;
    }

    order_moves(tactical_moves, board);

    for (const auto& move : tactical_moves) {
        board.make_move(move);
        int score = -quiescence(board, -beta, -alpha, nodes, start_time, time_limit_ms, stopped);
        board.undo_move(move);

        if (stopped) {
            return alpha;
        }

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

int Search::negamax(Board& board, int depth, int ply, int alpha, int beta, int& nodes, Move& best_move_root, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped) {
    ++nodes;

    if (check_time(nodes, start_time, time_limit_ms)) {
        stopped = true;
        return alpha;
    }

    if (depth <= 0) {
        return quiescence(board, alpha, beta, nodes, start_time, time_limit_ms, stopped);
    }

    auto moves = MoveGen::generate_legal_moves(board);

    if (moves.empty()) {
        if (MoveGen::is_in_check(board, board.side_to_move())) {
            return -MATE_SCORE + ply; // Faster mate preferred
        }
        return 0; // Stalemate
    }

    order_moves(moves, board);

    int best_score = -INFINITY_SCORE;
    Move current_best_move = moves.front();

    for (const auto& move : moves) {
        board.make_move(move);
        Move dummy;
        int score = -negamax(board, depth - 1, ply + 1, -beta, -alpha, nodes, dummy, start_time, time_limit_ms, stopped);
        board.undo_move(move);

        if (stopped) {
            return best_score;
        }

        if (score > best_score) {
            best_score = score;
            current_best_move = move;
            if (ply == 0) {
                best_move_root = move;
            }
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            break; // Beta cutoff (Fail-high)
        }
    }

    if (ply == 0) {
        best_move_root = current_best_move;
    }

    return best_score;
}

SearchResult Search::search(Board& board, int depth, int time_limit_ms) {
    SearchResult result;
    result.depth = 0;
    result.nodes = 0;

    auto legals = MoveGen::generate_legal_moves(board);
    if (legals.empty()) {
        return result;
    }
    Move best_move = legals.front();

    auto start_time = std::chrono::steady_clock::now();
    bool stopped = false;

    // Iterative Deepening from depth 1 to target depth
    for (int d = 1; d <= depth; ++d) {
        Move current_d_move;
        int score = negamax(board, d, 0, -INFINITY_SCORE, INFINITY_SCORE, result.nodes, current_d_move, start_time, time_limit_ms, stopped);

        if (stopped) {
            break; // Stop and use best_move from previous completed iteration
        }

        if (current_d_move.is_valid()) {
            best_move = current_d_move;
            result.score = score;
            result.depth = d;
        }

        // If checkmate found, can stop early
        if (score > MATE_SCORE - 100 || score < -MATE_SCORE + 100) {
            break;
        }

        // If we spent more than 40% of our allocated time on this iteration, starting the next deeper iteration will likely exceed time limit
        if (time_limit_ms > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
            if (elapsed >= (time_limit_ms * 4) / 10) {
                break;
            }
        }
    }

    result.best_move = best_move;
    return result;
}

Move Search::get_best_move(Board& board, int depth, int time_limit_ms) {
    return search(board, depth, time_limit_ms).best_move;
}

int Search::minmax_eval(Board &board, int depth) {
    if (board.side_to_move() == Color::White) {
        return Eval::evaluate(board);
    }
    else {
        auto x = Search::get_best_move(board, depth);
        board.make_move(x);
        auto res = Eval::evaluate(board);
        board.undo_move(x);
        return res;
    }
}
} // namespace chess
