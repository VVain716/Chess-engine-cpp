#include "core/Board.hpp"
#include "core/Move.hpp"

namespace chess {

class Eval {
public:
  static int get_game_phase(const Board &board);
  static int white_eval(const Board &board);
  static int black_eval(const Board &board);
  static int evaluate(const Board &board);
};

} // namespace chess
