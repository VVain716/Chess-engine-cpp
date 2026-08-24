#include "core/Search.hpp"
#include "core/Notation.hpp"
#include <algorithm>
#include <iostream>

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

bool Search::has_non_pawn_material(const Board& board, Color color) {
    for (Square sq = 0; sq < 64; ++sq) {
        Piece p = board.get_piece(sq);
        if (piece_color(p) == color) {
            PieceType pt = piece_type(p);
            if (pt != PieceType::None && pt != PieceType::Pawn && pt != PieceType::King) {
                return true;
            }
        }
    }
    return false;
}

int Search::score_move(const Move& move, const Board& board, const Move& tt_move, int ply) {
    if (tt_move.is_valid() && move == tt_move) {
        return 1000000;
    }

    Piece moving_piece = board.get_piece(move.from);
    Piece target_piece = board.get_piece(move.to);

    // 1. Captures (MVV-LVA: Most Valuable Victim - Least Valuable Attacker)
    if (target_piece != Piece::None) {
        int victim_val   = get_piece_value(piece_type(target_piece));
        int attacker_val = get_piece_value(piece_type(moving_piece));
        return 100000 + (victim_val * 10 - attacker_val);
    }

    if (move.type == MoveType::EnPassant) {
        return 100000 + (100 * 10 - 100);
    }

    // 2. Promotions
    if (move.type == MoveType::Promotion) {
        return 90000 + get_piece_value(move.promotion_piece);
    }

    // 3. Killer Moves
    if (ply < MAX_PLY) {
        if (move == killer_moves_[ply][0]) return 80000;
        if (move == killer_moves_[ply][1]) return 70000;
    }

    // 4. Castling
    if (move.is_castle()) {
        return 60000;
    }

    // 5. History Heuristic
    Color c = board.side_to_move();
    int color_idx = (c == Color::White ? 0 : 1);
    return history_table_[color_idx][move.from][move.to];
}

struct ScoredMove {
    Move move;
    int score = 0;
};

void Search::order_moves(std::vector<Move>& moves, const Board& board, const Move& tt_move, int ply) {
    if (moves.size() <= 1) return;

    std::vector<ScoredMove> scored_moves;
    scored_moves.reserve(moves.size());

    for (const auto& m : moves) {
        scored_moves.push_back({m, score_move(m, board, tt_move, ply)});
    }

    std::sort(scored_moves.begin(), scored_moves.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.score > b.score;
    });

    for (size_t i = 0; i < moves.size(); ++i) {
        moves[i] = scored_moves[i].move;
    }
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

int Search::quiescence(Board& board, int alpha, int beta, int& nodes, int ply, int& seldepth, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped) {
    ++nodes;
    if (ply > seldepth) seldepth = ply;

    if (check_time(nodes, start_time, time_limit_ms)) {
        stopped = true;
        return alpha;
    }

    if (ply >= MAX_PLY - 1) {
        return get_side_eval(board);
    }

    Color moving_side = board.side_to_move();
    bool in_check = MoveGen::is_in_check(board, moving_side);
    int stand_pat = get_side_eval(board);

    if (!in_check) {
        if (stand_pat >= beta) {
            return beta;
        }
        if (stand_pat > alpha) {
            alpha = stand_pat;
        }
    }

    auto pseudo_moves = MoveGen::generate_pseudo_legal_moves(board);

    std::vector<Move> tactical_moves;
    tactical_moves.reserve(pseudo_moves.size());

    for (const auto& m : pseudo_moves) {
        bool is_promo = (m.type == MoveType::Promotion);
        bool is_capture = (board.get_piece(m.to) != Piece::None || m.type == MoveType::EnPassant);
        if (in_check || is_promo || is_capture) {
            if (!in_check && !is_promo) {
                int victim_val = get_piece_value(piece_type(board.get_piece(m.to)));
                if (m.type == MoveType::EnPassant) victim_val = 100;
                if (stand_pat + victim_val + 200 < alpha) {
                    continue; // Delta pruning
                }
            }
            tactical_moves.push_back(m);
        }
    }

    order_moves(tactical_moves, board, Move{}, ply);

    int legal_moves = 0;
    for (const auto& move : tactical_moves) {
        board.make_move(move);
        if (MoveGen::is_in_check(board, moving_side)) {
            board.undo_move(move);
            continue;
        }
        legal_moves++;

        int score = -quiescence(board, -beta, -alpha, nodes, ply + 1, seldepth, start_time, time_limit_ms, stopped);
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

    if (in_check && legal_moves == 0) {
        // Double check if there are really no legal moves (checkmate)
        auto all_legals = MoveGen::generate_legal_moves(board);
        if (all_legals.empty()) {
            return -MATE_SCORE + ply;
        }
    }

    return alpha;
}

int Search::negamax(Board& board, int depth, int ply, int alpha, int beta, int& nodes, int& seldepth, Move& best_move_root, const std::chrono::steady_clock::time_point& start_time, int time_limit_ms, bool& stopped, bool allow_null) {
    ++nodes;
    if (ply > seldepth) seldepth = ply;

    if (check_time(nodes, start_time, time_limit_ms)) {
        stopped = true;
        return alpha;
    }

    Color moving_side = board.side_to_move();
    bool in_check = MoveGen::is_in_check(board, moving_side);

    // Check Extension: search checking evasions deeper
    if (in_check && depth > 0 && ply < 32) {
        depth++;
    }

    // 1. Transposition Table Probe
    uint64_t hash = board.hash();
    TTEntry tt_entry;
    bool tt_hit = tt_.probe(hash, tt_entry);
    Move tt_move{};

    if (tt_hit) {
        tt_move = tt_entry.best_move;
        if (ply > 0 && tt_entry.depth >= depth) {
            int tt_score = TranspositionTable::score_from_tt(tt_entry.score, ply);
            if (tt_entry.flag == TTFlag::Exact) {
                return tt_score;
            } else if (tt_entry.flag == TTFlag::LowerBound) {
                if (tt_score >= beta) return tt_score;
            } else if (tt_entry.flag == TTFlag::UpperBound) {
                if (tt_score <= alpha) return tt_score;
            }
        }
    }

    if (depth <= 0) {
        return quiescence(board, alpha, beta, nodes, ply, seldepth, start_time, time_limit_ms, stopped);
    }

    // 2. Null Move Pruning (NMP)
    if (allow_null && !in_check && depth >= 3 && ply > 0 && has_non_pawn_material(board, moving_side)) {
        int R = 2 + (depth >= 6 ? 1 : 0);
        board.make_null_move();
        Move null_dummy;
        int null_score = -negamax(board, depth - 1 - R, ply + 1, -beta, -beta + 1, nodes, seldepth, null_dummy, start_time, time_limit_ms, stopped, false);
        board.undo_null_move();

        if (stopped) return beta;
        if (null_score >= beta) {
            return (null_score >= MATE_SCORE - 100) ? beta : null_score;
        }
    }

    auto pseudo_moves = MoveGen::generate_pseudo_legal_moves(board);

    // Order moves with TT move, killers, and history heuristic
    order_moves(pseudo_moves, board, tt_move, ply);

    int alpha_orig = alpha;
    int best_score = -INFINITY_SCORE;
    Move current_best_move;
    int legal_moves_count = 0;

    for (const auto& move : pseudo_moves) {
        board.make_move(move);
        if (MoveGen::is_in_check(board, moving_side)) {
            board.undo_move(move);
            continue;
        }
        legal_moves_count++;
        if (!current_best_move.is_valid()) {
            current_best_move = move;
        }

        int score = 0;
        bool is_capture = (board.get_piece(move.to) != Piece::None || move.type == MoveType::EnPassant || move.type == MoveType::Promotion);

        // 3. Late Move Reductions (LMR)
        if (legal_moves_count >= 4 && depth >= 3 && !in_check && !is_capture) {
            int reduction = 1 + (legal_moves_count >= 8 ? 1 : 0);
            int reduced_depth = std::max(1, depth - 1 - reduction);
            Move dummy;
            score = -negamax(board, reduced_depth, ply + 1, -alpha - 1, -alpha, nodes, seldepth, dummy, start_time, time_limit_ms, stopped, true);
            if (score > alpha && !stopped) {
                // Re-search at full depth
                score = -negamax(board, depth - 1, ply + 1, -beta, -alpha, nodes, seldepth, dummy, start_time, time_limit_ms, stopped, true);
            }
        } else {
            Move dummy;
            score = -negamax(board, depth - 1, ply + 1, -beta, -alpha, nodes, seldepth, dummy, start_time, time_limit_ms, stopped, true);
        }

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
            // Beta cutoff (Fail-high) -> Update Killers & History for quiet moves
            if (!is_capture) {
                if (ply < MAX_PLY) {
                    if (killer_moves_[ply][0] != move) {
                        killer_moves_[ply][1] = killer_moves_[ply][0];
                        killer_moves_[ply][0] = move;
                    }
                }
                int color_idx = (moving_side == Color::White ? 0 : 1);
                history_table_[color_idx][move.from][move.to] += depth * depth;
                if (history_table_[color_idx][move.from][move.to] > 50000) {
                    for (int col = 0; col < 2; ++col)
                        for (int f = 0; f < 64; ++f)
                            for (int t = 0; t < 64; ++t)
                                history_table_[col][f][t] /= 2;
                }
            }
            break;
        }
    }

    if (legal_moves_count == 0) {
        if (in_check) {
            return -MATE_SCORE + ply; // Faster mate preferred
        }
        return 0; // Stalemate
    }

    if (ply == 0 && current_best_move.is_valid()) {
        best_move_root = current_best_move;
    }

    // 4. Transposition Table Store
    if (!stopped) {
        TTFlag flag;
        if (best_score >= beta) {
            flag = TTFlag::LowerBound;
        } else if (best_score > alpha_orig) {
            flag = TTFlag::Exact;
        } else {
            flag = TTFlag::UpperBound;
        }
        tt_.store(hash, depth, ply, best_score, flag, current_best_move);
    }

    return best_score;
}

std::vector<Move> Search::get_pv_line(Board& board, int max_depth) {
    std::vector<Move> pv;
    std::vector<uint64_t> visited_hashes;

    for (int d = 0; d < max_depth; ++d) {
        uint64_t hash = board.hash();
        if (std::find(visited_hashes.begin(), visited_hashes.end(), hash) != visited_hashes.end()) {
            break; // Loop / repetition detected
        }
        visited_hashes.push_back(hash);

        TTEntry entry;
        if (!tt_.probe(hash, entry) || !entry.best_move.is_valid()) {
            break;
        }

        auto legals = MoveGen::generate_legal_moves(board);
        auto it = std::find_if(legals.begin(), legals.end(), [&](const Move& m) {
            return m.from == entry.best_move.from && m.to == entry.best_move.to && m.type == entry.best_move.type;
        });
        if (it == legals.end()) {
            break;
        }

        pv.push_back(*it);
        board.make_move(*it);
    }

    // Undo all made moves in reverse order
    for (auto it = pv.rbegin(); it != pv.rend(); ++it) {
        board.undo_move(*it);
    }

    return pv;
}

SearchResult Search::search(Board& board, int depth, int time_limit_ms, bool output_uci_info) {
    SearchResult result;
    result.depth = 0;
    result.nodes = 0;

    auto legals = MoveGen::generate_legal_moves(board);
    if (legals.empty()) {
        return result;
    }
    Move best_move = legals.front();

    tt_.new_search();
    for (int p = 0; p < MAX_PLY; ++p) {
        killer_moves_[p][0] = Move{};
        killer_moves_[p][1] = Move{};
    }

    auto start_time = std::chrono::steady_clock::now();
    bool stopped = false;

    // Iterative Deepening from depth 1 to target depth
    for (int d = 1; d <= depth; ++d) {
        Move current_d_move;
        int seldepth = d;
        int score = negamax(board, d, 0, -INFINITY_SCORE, INFINITY_SCORE, result.nodes, seldepth, current_d_move, start_time, time_limit_ms, stopped, true);

        if (stopped && d > 1) {
            break; // Stop and use best_move from previous completed iteration
        }

        if (current_d_move.is_valid()) {
            best_move = current_d_move;
            result.score = score;
            result.depth = d;
            result.seldepth = seldepth;
        }

        auto now = std::chrono::steady_clock::now();
        int elapsed_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count());
        uint64_t nps = (elapsed_ms > 0) ? (static_cast<uint64_t>(result.nodes) * 1000ULL / elapsed_ms) : (static_cast<uint64_t>(result.nodes) * 1000ULL);
        result.time_ms = elapsed_ms;
        result.nps = nps;
        result.hashfull = tt_.hashfull();
        result.pv = get_pv_line(board, d);
        if (result.pv.empty() && best_move.is_valid()) {
            result.pv.push_back(best_move);
        }

        if (output_uci_info) {
            std::cout << "info depth " << d << " seldepth " << seldepth << " score ";
            if (score > MATE_SCORE - 1000) {
                int mate_moves = (MATE_SCORE - score + 1) / 2;
                std::cout << "mate " << mate_moves;
            } else if (score < -MATE_SCORE + 1000) {
                int mate_moves = -(MATE_SCORE + score + 1) / 2;
                std::cout << "mate " << mate_moves;
            } else {
                std::cout << "cp " << score;
            }
            std::cout << " nodes " << result.nodes << " nps " << nps << " time " << elapsed_ms
                      << " hashfull " << result.hashfull << " pv";
            for (const auto& m : result.pv) {
                std::cout << " " << square_to_algebraic(m.from) << square_to_algebraic(m.to);
                if (m.type == MoveType::Promotion) {
                    char promo_c = 'q';
                    switch (m.promotion_piece) {
                        case PieceType::Knight: promo_c = 'n'; break;
                        case PieceType::Bishop: promo_c = 'b'; break;
                        case PieceType::Rook:   promo_c = 'r'; break;
                        case PieceType::Queen:  promo_c = 'q'; break;
                        default: break;
                    }
                    std::cout << promo_c;
                }
            }
            std::cout << "\n" << std::flush;
        }

        // If checkmate found, can stop early
        if (score > MATE_SCORE - 100 || score < -MATE_SCORE + 100) {
            break;
        }

        // If we spent more than 40% of our allocated time on this iteration, starting next deeper iteration will likely exceed time limit
        if (time_limit_ms > 0) {
            if (elapsed_ms >= (time_limit_ms * 4) / 10) {
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
    SearchResult result = search(board, depth);
    return (board.side_to_move() == Color::White) ? result.score : -result.score;
}

} // namespace chess
