#pragma once

#include "core/Board.hpp"
#include "core/Move.hpp"
#include <string>

namespace chess {

class MoveHistory {
private:
    std::string history_str_;
    int move_number_ = 1;

public:
    MoveHistory() = default;

    void record_move(const Board& board_before_move, const Move& move);
    void record_white_win();
    void record_black_win();
    void record_draw();

    [[nodiscard]] const std::string& get_notation() const { return history_str_; }
    void clear();

    static std::string format_move(const Board& board_before_move, const Move& move);
};

using Notation = MoveHistory;

} // namespace chess
