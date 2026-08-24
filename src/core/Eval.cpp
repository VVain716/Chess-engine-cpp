#include "core/Eval.hpp"

namespace chess {

// ============================================================================
// Middlegame Piece-Square Tables (Base Material + Positional Centipawns)
// Indexed by [row][col] where row 0 = Rank 8, row 7 = Rank 1
// ============================================================================

// Base Pawn: 100 cp
const int Eval::Middlegame::pawn_table[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},                 // Rank 8 (Promoted)
    {150, 150, 150, 150, 150, 150, 150, 150}, // Rank 7 (Strong advance)
    {110, 110, 120, 130, 130, 120, 110, 110}, // Rank 6
    {105, 105, 110, 125, 125, 110, 105, 105}, // Rank 5
    {100, 100, 100, 120, 120, 100, 100, 100}, // Rank 4 (Center push)
    {105, 95, 90, 100, 100, 90, 95, 105},     // Rank 3
    {105, 110, 110, 80, 80, 110, 110, 105},   // Rank 2 (Starting rank)
    {0, 0, 0, 0, 0, 0, 0, 0}                  // Rank 1
};

// Base Knight: 320 cp
const int Eval::Middlegame::knight_table[8][8] = {
    {270, 280, 290, 290, 290, 290, 280, 270}, // Rank 8
    {280, 300, 320, 320, 320, 320, 300, 280}, // Rank 7
    {290, 320, 335, 340, 340, 335, 320, 290}, // Rank 6
    {290, 325, 340, 345, 345, 340, 325, 290}, // Rank 5 (Central outpost)
    {290, 320, 340, 345, 345, 340, 320, 290}, // Rank 4 (Central outpost)
    {290, 325, 330, 335, 335, 330, 325, 290}, // Rank 3
    {280, 300, 320, 325, 325, 320, 300, 280}, // Rank 2 (Developing squares)
    {270, 280, 290, 290, 290, 290, 280, 270}  // Rank 1 (Starting rank)
};

// Base Bishop: 330 cp
const int Eval::Middlegame::bishop_table[8][8] = {
    {310, 320, 320, 320, 320, 320, 320, 310}, // Rank 8
    {320, 335, 330, 330, 330, 330, 335, 320}, // Rank 7
    {320, 330, 340, 340, 340, 340, 330, 320}, // Rank 6
    {320, 335, 340, 345, 345, 340, 335, 320}, // Rank 5 (Active diagonals)
    {320, 330, 340, 345, 345, 340, 330, 320}, // Rank 4
    {320, 340, 335, 335, 335, 335, 340, 320}, // Rank 3
    {320, 340, 330, 335, 335, 330, 340, 320}, // Rank 2 (Fianchetto/development)
    {310, 320, 320, 320, 320, 320, 320, 310}  // Rank 1
};

// Base Rook: 500 cp
const int Eval::Middlegame::rook_table[8][8] = {
    {500, 500, 500, 500, 500, 500, 500, 500}, // Rank 8
    {505, 510, 510, 510, 510, 510, 510, 505}, // Rank 7 (7th rank attack)
    {495, 500, 500, 500, 500, 500, 500, 495}, // Rank 6
    {495, 500, 500, 500, 500, 500, 500, 495}, // Rank 5
    {495, 500, 500, 500, 500, 500, 500, 495}, // Rank 4
    {495, 500, 500, 500, 500, 500, 500, 495}, // Rank 3
    {495, 500, 500, 500, 500, 500, 500, 495}, // Rank 2
    {500, 500, 505, 510, 510, 505, 500, 500}  // Rank 1 (Central d/e files)
};

// Base Queen: 900 cp
const int Eval::Middlegame::queen_table[8][8] = {
    {880, 890, 890, 895, 895, 890, 890, 880}, // Rank 8
    {890, 900, 900, 900, 900, 900, 900, 890}, // Rank 7
    {890, 900, 905, 905, 905, 905, 900, 890}, // Rank 6
    {895, 900, 905, 905, 905, 905, 900, 895}, // Rank 5
    {900, 900, 905, 905, 905, 905, 900, 900}, // Rank 4
    {890, 905, 905, 905, 905, 905, 905, 890}, // Rank 3
    {890, 900, 905, 900, 900, 900, 900, 890}, // Rank 2
    {880, 890, 890, 895, 895, 890, 890, 880}  // Rank 1
};

// Base King: 20000 cp (Safety & Shelter)
const int Eval::Middlegame::king_table[8][8] = {
    {19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970}, // Rank 8
    {19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970}, // Rank 7
    {19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970}, // Rank 6
    {19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970}, // Rank 5
    {19980, 19970, 19970, 19960, 19960, 19970, 19970, 19980}, // Rank 4
    {19990, 19980, 19980, 19980, 19980, 19980, 19980, 19990}, // Rank 3
    {20020, 20020, 20000, 20000, 20000, 20000, 20020, 20020}, // Rank 2
    {20020, 20030, 20010, 20000, 20000, 20010, 20030,
     20020} // Rank 1 (Castled g1/c1)
};

// ============================================================================
// Endgame Piece-Square Tables (Base Material + Positional Centipawns)
// Indexed by [row][col] where row 0 = Rank 8, row 7 = Rank 1
// ============================================================================

// Base Pawn: 120 cp (Rewards promotion advancement)
const int Eval::Endgame::pawn_table[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},                 // Rank 8 (Queened)
    {200, 200, 200, 200, 200, 200, 200, 200}, // Rank 7 (Imminent promotion)
    {170, 170, 170, 170, 170, 170, 170, 170}, // Rank 6
    {150, 150, 150, 150, 150, 150, 150, 150}, // Rank 5
    {135, 135, 135, 135, 135, 135, 135, 135}, // Rank 4
    {125, 125, 125, 125, 125, 125, 125, 125}, // Rank 3
    {120, 120, 120, 120, 120, 120, 120, 120}, // Rank 2
    {0, 0, 0, 0, 0, 0, 0, 0}                  // Rank 1
};

// Base Knight: 300 cp
const int Eval::Endgame::knight_table[8][8] = {
    {250, 270, 280, 280, 280, 280, 270, 250}, // Rank 8
    {270, 285, 300, 300, 300, 300, 285, 270}, // Rank 7
    {280, 300, 315, 320, 320, 315, 300, 280}, // Rank 6
    {280, 305, 320, 325, 325, 320, 305, 280}, // Rank 5
    {280, 305, 320, 325, 325, 320, 305, 280}, // Rank 4
    {280, 300, 315, 320, 320, 315, 300, 280}, // Rank 3
    {270, 285, 300, 300, 300, 300, 285, 270}, // Rank 2
    {250, 270, 280, 280, 280, 280, 270, 250}  // Rank 1
};

// Base Bishop: 330 cp
const int Eval::Endgame::bishop_table[8][8] = {
    {310, 320, 320, 320, 320, 320, 320, 310}, // Rank 8
    {320, 330, 330, 330, 330, 330, 330, 320}, // Rank 7
    {320, 330, 340, 340, 340, 340, 330, 320}, // Rank 6
    {320, 330, 340, 345, 345, 340, 330, 320}, // Rank 5
    {320, 330, 340, 345, 345, 340, 330, 320}, // Rank 4
    {320, 330, 340, 340, 340, 340, 330, 320}, // Rank 3
    {320, 330, 330, 330, 330, 330, 330, 320}, // Rank 2
    {310, 320, 320, 320, 320, 320, 320, 310}  // Rank 1
};

// Base Rook: 530 cp
const int Eval::Endgame::rook_table[8][8] = {
    {530, 530, 530, 530, 530, 530, 530, 530}, // Rank 8
    {535, 540, 540, 540, 540, 540, 540, 535}, // Rank 7
    {525, 530, 530, 530, 530, 530, 530, 525}, // Rank 6
    {525, 530, 530, 530, 530, 530, 530, 525}, // Rank 5
    {525, 530, 530, 530, 530, 530, 530, 525}, // Rank 4
    {525, 530, 530, 530, 530, 530, 530, 525}, // Rank 3
    {525, 530, 530, 530, 530, 530, 530, 525}, // Rank 2
    {530, 530, 530, 530, 530, 530, 530, 530}  // Rank 1
};

// Base Queen: 950 cp
const int Eval::Endgame::queen_table[8][8] = {
    {930, 940, 940, 945, 945, 940, 940, 930}, // Rank 8
    {940, 950, 950, 950, 950, 950, 950, 940}, // Rank 7
    {940, 950, 960, 960, 960, 960, 950, 940}, // Rank 6
    {945, 950, 960, 965, 965, 960, 950, 945}, // Rank 5
    {945, 950, 960, 965, 965, 960, 950, 945}, // Rank 4
    {940, 950, 960, 960, 960, 960, 950, 940}, // Rank 3
    {940, 950, 950, 950, 950, 950, 950, 940}, // Rank 2
    {930, 940, 940, 945, 945, 940, 940, 930}  // Rank 1
};

// Base King: 20000 cp (Centralization & Activity)
const int Eval::Endgame::king_table[8][8] = {
    {19950, 19970, 19970, 19970, 19970, 19970, 19970, 19950}, // Rank 8
    {19970, 19980, 19990, 19990, 19990, 19990, 19980, 19970}, // Rank 7
    {19970, 19990, 20020, 20030, 20030, 20020, 19990, 19970}, // Rank 6
    {19970, 19990, 20030, 20040, 20040, 20030, 19990,
     19970}, // Rank 5 (Central King)
    {19970, 19990, 20030, 20040, 20040, 20030, 19990,
     19970}, // Rank 4 (Central King)
    {19970, 19990, 20020, 20030, 20030, 20020, 19990, 19970}, // Rank 3
    {19970, 19980, 19990, 19990, 19990, 19990, 19980, 19970}, // Rank 2
    {19950, 19970, 19970, 19970, 19970, 19970, 19970, 19950}  // Rank 1
};

// ============================================================================
// Middlegame Helper Value Getters
// ============================================================================

int Eval::Middlegame::PawnValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return pawn_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::KnightValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return knight_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::BishopValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return bishop_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::RookValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return rook_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::QueenValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return queen_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::KingValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return king_table[square_row(pos)][square_col(pos)];
}

int Eval::Middlegame::PawnValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return pawn_table[row][col];
}

int Eval::Middlegame::KnightValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return knight_table[row][col];
}

int Eval::Middlegame::BishopValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return bishop_table[row][col];
}

int Eval::Middlegame::RookValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return rook_table[row][col];
}

int Eval::Middlegame::QueenValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return queen_table[row][col];
}

int Eval::Middlegame::KingValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return king_table[row][col];
}

// ============================================================================
// Endgame Helper Value Getters
// ============================================================================

int Eval::Endgame::PawnValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return pawn_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::KnightValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return knight_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::BishopValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return bishop_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::RookValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return rook_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::QueenValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return queen_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::KingValue(Square pos) {
  if (!is_valid_square(pos))
    return 0;
  return king_table[square_row(pos)][square_col(pos)];
}

int Eval::Endgame::PawnValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return pawn_table[row][col];
}

int Eval::Endgame::KnightValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return knight_table[row][col];
}

int Eval::Endgame::BishopValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return bishop_table[row][col];
}

int Eval::Endgame::RookValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return rook_table[row][col];
}

int Eval::Endgame::QueenValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return queen_table[row][col];
}

int Eval::Endgame::KingValue(int row, int col) {
  if (!is_valid_square(row, col))
    return 0;
  return king_table[row][col];
}

// ============================================================================
// Phase & Positional / Structural Evaluations
// ============================================================================

int Eval::get_game_phase(const Board &board) {
  int piece_count = 0;
  for (Square sq = 0; sq < 64; ++sq) {
    Piece piece = board.get_piece(sq);
    switch (piece) {
    case Piece::BlackKnight:
    case Piece::WhiteKnight:
    case Piece::BlackBishop:
    case Piece::WhiteBishop:
      piece_count += 1;
      break;
    case Piece::BlackRook:
    case Piece::WhiteRook:
      piece_count += 2;
      break;
    case Piece::BlackQueen:
    case Piece::WhiteQueen:
      piece_count += 4;
      break;
    default:
      break;
    }
  }
  return piece_count;
}

std::pair<int, int> Eval::evaluate_pawns(const Board &board, Color color) {
  int mg = 0;
  int eg = 0;

  Color opp_color = opponent_color(color);
  Piece friendly_pawn = make_piece(color, PieceType::Pawn);
  Piece enemy_pawn = make_piece(opp_color, PieceType::Pawn);

  int friendly_pawn_counts[8] = {0};
  int enemy_pawn_counts[8] = {0};
  std::vector<std::pair<int, int>> friendly_pawns;
  friendly_pawns.reserve(8);

  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      Piece p = board.get_piece(r, c);
      if (p == friendly_pawn) {
        friendly_pawn_counts[c]++;
        friendly_pawns.push_back({r, c});
      } else if (p == enemy_pawn) {
        enemy_pawn_counts[c]++;
      }
    }
  }

  // 1. Doubled Pawns
  for (int c = 0; c < 8; ++c) {
    if (friendly_pawn_counts[c] > 1) {
      int extra = friendly_pawn_counts[c] - 1;
      mg -= extra * 15;
      eg -= extra * 20;
    }
  }

  // 2. Pawn Islands
  int islands = 0;
  bool in_island = false;
  for (int c = 0; c < 8; ++c) {
    if (friendly_pawn_counts[c] > 0) {
      if (!in_island) {
        islands++;
        in_island = true;
      }
    } else {
      in_island = false;
    }
  }
  if (islands > 1) {
    mg -= (islands - 1) * 12;
    eg -= (islands - 1) * 15;
  }

  // 3. Isolated, Backward, Passed & Protected Passed Pawns
  for (const auto &[r, c] : friendly_pawns) {
    // Isolated pawn check
    bool left_has = (c > 0 && friendly_pawn_counts[c - 1] > 0);
    bool right_has = (c < 7 && friendly_pawn_counts[c + 1] > 0);
    bool is_isolated = (!left_has && !right_has);

    if (is_isolated) {
      mg -= 15;
      eg -= 20;
    } else {
      // Backward pawn check: no friendly pawn on adjacent files at/behind this rank
      bool has_friendly_behind_or_same = false;
      if (color == Color::White) {
        for (const auto &[fr, fc] : friendly_pawns) {
          if ((fc == c - 1 || fc == c + 1) && fr >= r) {
            has_friendly_behind_or_same = true;
            break;
          }
        }
      } else {
        for (const auto &[fr, fc] : friendly_pawns) {
          if ((fc == c - 1 || fc == c + 1) && fr <= r) {
            has_friendly_behind_or_same = true;
            break;
          }
        }
      }

      if (!has_friendly_behind_or_same) {
        // Stop square in front attacked by enemy pawn
        bool stop_attacked = false;
        if (color == Color::White) {
          if (r - 2 >= 0) {
            if (c - 1 >= 0 && board.get_piece(r - 2, c - 1) == enemy_pawn)
              stop_attacked = true;
            if (c + 1 < 8 && board.get_piece(r - 2, c + 1) == enemy_pawn)
              stop_attacked = true;
          }
        } else {
          if (r + 2 < 8) {
            if (c - 1 >= 0 && board.get_piece(r + 2, c - 1) == enemy_pawn)
              stop_attacked = true;
            if (c + 1 < 8 && board.get_piece(r + 2, c + 1) == enemy_pawn)
              stop_attacked = true;
          }
        }

        if (stop_attacked) {
          mg -= 15;
          eg -= 15;
        }
      }
    }

    // Passed pawn check: no enemy pawns on c-1, c, c+1 ahead
    bool is_passed = true;
    if (color == Color::White) {
      for (int er = 0; er < r; ++er) {
        if (c - 1 >= 0 && board.get_piece(er, c - 1) == enemy_pawn) {
          is_passed = false;
          break;
        }
        if (board.get_piece(er, c) == enemy_pawn) {
          is_passed = false;
          break;
        }
        if (c + 1 < 8 && board.get_piece(er, c + 1) == enemy_pawn) {
          is_passed = false;
          break;
        }
      }
    } else {
      for (int er = r + 1; er < 8; ++er) {
        if (c - 1 >= 0 && board.get_piece(er, c - 1) == enemy_pawn) {
          is_passed = false;
          break;
        }
        if (board.get_piece(er, c) == enemy_pawn) {
          is_passed = false;
          break;
        }
        if (c + 1 < 8 && board.get_piece(er, c + 1) == enemy_pawn) {
          is_passed = false;
          break;
        }
      }
    }

    if (is_passed) {
      int rank = (color == Color::White ? (7 - r) : r);
      constexpr int passed_mg[8] = {0, 0, 5, 10, 20, 40, 70, 120};
      constexpr int passed_eg[8] = {0, 0, 10, 20, 35, 60, 100, 160};
      if (rank >= 0 && rank < 8) {
        mg += passed_mg[rank];
        eg += passed_eg[rank];
      }

      // Protected passed pawn check
      bool is_protected = false;
      if (color == Color::White) {
        if (r + 1 < 8) {
          if (c - 1 >= 0 && board.get_piece(r + 1, c - 1) == friendly_pawn)
            is_protected = true;
          if (c + 1 < 8 && board.get_piece(r + 1, c + 1) == friendly_pawn)
            is_protected = true;
        }
      } else {
        if (r - 1 >= 0) {
          if (c - 1 >= 0 && board.get_piece(r - 1, c - 1) == friendly_pawn)
            is_protected = true;
          if (c + 1 < 8 && board.get_piece(r - 1, c + 1) == friendly_pawn)
            is_protected = true;
        }
      }

      if (is_protected) {
        mg += 20;
        eg += 30;
      }
    }
  }

  return {mg, eg};
}

std::pair<int, int> Eval::evaluate_rooks(const Board &board, Color color) {
  int mg = 0;
  int eg = 0;

  Color opp_color = opponent_color(color);
  Piece friendly_rook = make_piece(color, PieceType::Rook);
  Piece friendly_pawn = make_piece(color, PieceType::Pawn);
  Piece enemy_pawn = make_piece(opp_color, PieceType::Pawn);
  Square enemy_king_sq = board.king_square(opp_color);

  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (board.get_piece(r, c) != friendly_rook)
        continue;

      // 1. Open and Semi-Open Files
      int friendly_pawns = 0;
      int enemy_pawns = 0;
      for (int row_i = 0; row_i < 8; ++row_i) {
        Piece p = board.get_piece(row_i, c);
        if (p == friendly_pawn)
          friendly_pawns++;
        else if (p == enemy_pawn)
          enemy_pawns++;
      }

      if (friendly_pawns == 0 && enemy_pawns == 0) {
        mg += 25;
        eg += 20; // Open file
      } else if (friendly_pawns == 0 && enemy_pawns > 0) {
        mg += 12;
        eg += 10; // Semi-open file
      }

      // 2. 7th Rank
      bool is_7th = (color == Color::White ? (r == 1) : (r == 6));
      if (is_7th) {
        mg += 30;
        eg += 40;
        if (is_valid_square(enemy_king_sq)) {
          int ek_row = square_row(enemy_king_sq);
          if ((color == Color::White && ek_row <= 1) ||
              (color == Color::Black && ek_row >= 6)) {
            mg += 10;
            eg += 15;
          }
        }
      }

      // 3. Trapped Rook in Corner
      int mobility = 0;
      constexpr int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
      for (const auto &[dr, dc] : dirs) {
        int cr = r + dr;
        int cc = c + dc;
        while (is_valid_square(cr, cc)) {
          Piece p = board.get_piece(cr, cc);
          if (p == Piece::None) {
            mobility++;
          } else {
            if (piece_color(p) != color)
              mobility++;
            break;
          }
          cr += dr;
          cc += dc;
        }
      }

      if (mobility <= 1) {
        if ((color == Color::White && r >= 6 &&
             (c == 0 || c == 7 || c == 1 || c == 6)) ||
            (color == Color::Black && r <= 1 &&
             (c == 0 || c == 7 || c == 1 || c == 6))) {
          mg -= 30;
          eg -= 20;
        }
      }
    }
  }

  return {mg, eg};
}

std::pair<int, int> Eval::evaluate_bishops(const Board &board, Color color,
                                           int total_pawns) {
  int mg = 0;
  int eg = 0;

  Piece friendly_bishop = make_piece(color, PieceType::Bishop);

  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (board.get_piece(r, c) != friendly_bishop)
        continue;

      // 1. Mobility and Long Diagonals
      int mobility = 0;
      constexpr int dirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
      for (const auto &[dr, dc] : dirs) {
        int cr = r + dr;
        int cc = c + dc;
        while (is_valid_square(cr, cc)) {
          Piece p = board.get_piece(cr, cc);
          if (p == Piece::None) {
            mobility++;
          } else {
            if (piece_color(p) != color)
              mobility++;
            break;
          }
          cr += dr;
          cc += dc;
        }
      }

      mg += mobility * 4;
      eg += mobility * 4;

      // Long diagonal bonus (major diagonals of length 7 or 8)
      int diff = r - c;
      int sum = r + c;
      if (std::abs(diff) <= 1 || sum == 6 || sum == 7 || sum == 8) {
        if (mobility >= 4) {
          mg += 10;
          eg += 10;
        }
      }

      // 2. Closed Positions Penalty
      if (total_pawns >= 12) {
        mg -= 15;
        eg -= 15;
      }
      bool e_locked = (board.get_piece(4, 4) == Piece::WhitePawn &&
                       board.get_piece(3, 4) == Piece::BlackPawn);
      bool d_locked = (board.get_piece(4, 3) == Piece::WhitePawn &&
                       board.get_piece(3, 3) == Piece::BlackPawn);
      if (e_locked || d_locked) {
        mg -= 10;
        eg -= 10;
      }
    }
  }

  return {mg, eg};
}

std::pair<int, int> Eval::evaluate_knights(const Board &board, Color color,
                                           int total_pawns) {
  int mg = 0;
  int eg = 0;

  Piece friendly_knight = make_piece(color, PieceType::Knight);
  Piece friendly_pawn = make_piece(color, PieceType::Pawn);
  Piece enemy_pawn = make_piece(opponent_color(color), PieceType::Pawn);

  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (board.get_piece(r, c) != friendly_knight)
        continue;

      // 1. Centralization
      if (r >= 2 && r <= 5 && c >= 2 && c <= 5) {
        mg += 10;
        eg += 5;
      }

      // 2. Outposts (ranks 4, 5, 6 defended by pawn and cannot be attacked by enemy pawns)
      bool is_outpost_rank = (color == Color::White ? (r >= 2 && r <= 4)
                                                    : (r >= 3 && r <= 5));
      if (is_outpost_rank && c >= 1 && c <= 6) {
        bool defended_by_pawn = false;
        if (color == Color::White) {
          if ((r + 1 < 8 && c - 1 >= 0 &&
               board.get_piece(r + 1, c - 1) == friendly_pawn) ||
              (r + 1 < 8 && c + 1 < 8 &&
               board.get_piece(r + 1, c + 1) == friendly_pawn)) {
            defended_by_pawn = true;
          }
        } else {
          if ((r - 1 >= 0 && c - 1 >= 0 &&
               board.get_piece(r - 1, c - 1) == friendly_pawn) ||
              (r - 1 >= 0 && c + 1 < 8 &&
               board.get_piece(r - 1, c + 1) == friendly_pawn)) {
            defended_by_pawn = true;
          }
        }

        if (defended_by_pawn) {
          bool can_be_attacked_by_pawn = false;
          if (color == Color::White) {
            for (int er = 0; er <= r; ++er) {
              if (c - 1 >= 0 && board.get_piece(er, c - 1) == enemy_pawn)
                can_be_attacked_by_pawn = true;
              if (c + 1 < 8 && board.get_piece(er, c + 1) == enemy_pawn)
                can_be_attacked_by_pawn = true;
            }
          } else {
            for (int er = r; er < 8; ++er) {
              if (c - 1 >= 0 && board.get_piece(er, c - 1) == enemy_pawn)
                can_be_attacked_by_pawn = true;
              if (c + 1 < 8 && board.get_piece(er, c + 1) == enemy_pawn)
                can_be_attacked_by_pawn = true;
            }
          }

          if (!can_be_attacked_by_pawn) {
            mg += 30;
            eg += 20;
          }
        }
      }

      // 3. Closed vs Open positions
      if (total_pawns >= 12) {
        mg += 15;
        eg += 10;
      } else if (total_pawns <= 8) {
        mg -= 15;
        eg -= 15;
      }
    }
  }

  return {mg, eg};
}

std::pair<int, int> Eval::evaluate_king(const Board &board, Color color) {
  int mg = 0;
  int eg = 0;

  Square king_sq = board.king_square(color);
  if (!is_valid_square(king_sq))
    return {0, 0};

  int kr = square_row(king_sq);
  int kc = square_col(king_sq);
  Piece friendly_pawn = make_piece(color, PieceType::Pawn);
  Piece enemy_pawn = make_piece(opponent_color(color), PieceType::Pawn);

  // 1. Middlegame King Pawn Shield
  if (color == Color::White && kr >= 5) {
    for (int c = std::max(0, kc - 1); c <= std::min(7, kc + 1); ++c) {
      bool shield = false;
      for (int r = kr - 1; r >= std::max(0, kr - 2); --r) {
        if (board.get_piece(r, c) == friendly_pawn) {
          shield = true;
          break;
        }
      }
      if (!shield) {
        mg -= 15;
      }
    }
  } else if (color == Color::Black && kr <= 2) {
    for (int c = std::max(0, kc - 1); c <= std::min(7, kc + 1); ++c) {
      bool shield = false;
      for (int r = kr + 1; r <= std::min(7, kr + 2); ++r) {
        if (board.get_piece(r, c) == friendly_pawn) {
          shield = true;
          break;
        }
      }
      if (!shield) {
        mg -= 15;
      }
    }
  }

  // 2. King on Open / Semi-Open Files (Middlegame)
  int friendly_pawns = 0;
  int enemy_pawns = 0;
  for (int r = 0; r < 8; ++r) {
    Piece p = board.get_piece(r, kc);
    if (p == friendly_pawn)
      friendly_pawns++;
    else if (p == enemy_pawn)
      enemy_pawns++;
  }
  if (friendly_pawns == 0 && enemy_pawns == 0) {
    mg -= 35;
  } else if (friendly_pawns == 0 && enemy_pawns > 0) {
    mg -= 20;
  }

  // 3. Endgame King Centralization
  int dist_r = std::min(std::abs(kr - 3), std::abs(kr - 4));
  int dist_c = std::min(std::abs(kc - 3), std::abs(kc - 4));
  int center_dist = dist_r + dist_c;
  eg += (6 - center_dist) * 5;

  return {mg, eg};
}

int Eval::white_eval(const Board &board) {
  int mg = 0;
  int eg = 0;
  int total_pawns = 0;

  for (Square sq = 0; sq < 64; ++sq) {
    Piece piece = board.get_piece(sq);
    if (piece == Piece::None)
      continue;

    if (piece_type(piece) == PieceType::Pawn)
      total_pawns++;

    if (piece_color(piece) != Color::White)
      continue;

    switch (piece_type(piece)) {
    case PieceType::Pawn:
      mg += Middlegame::PawnValue(sq);
      eg += Endgame::PawnValue(sq);
      break;
    case PieceType::Knight:
      mg += Middlegame::KnightValue(sq);
      eg += Endgame::KnightValue(sq);
      break;
    case PieceType::Bishop:
      mg += Middlegame::BishopValue(sq);
      eg += Endgame::BishopValue(sq);
      break;
    case PieceType::Rook:
      mg += Middlegame::RookValue(sq);
      eg += Endgame::RookValue(sq);
      break;
    case PieceType::Queen:
      mg += Middlegame::QueenValue(sq);
      eg += Endgame::QueenValue(sq);
      break;
    case PieceType::King:
      mg += Middlegame::KingValue(sq);
      eg += Endgame::KingValue(sq);
      break;
    default:
      break;
    }
  }

  // Positional and structural terms
  auto [pawn_mg, pawn_eg] = evaluate_pawns(board, Color::White);
  auto [rook_mg, rook_eg] = evaluate_rooks(board, Color::White);
  auto [bishop_mg, bishop_eg] =
      evaluate_bishops(board, Color::White, total_pawns);
  auto [knight_mg, knight_eg] =
      evaluate_knights(board, Color::White, total_pawns);
  auto [king_mg, king_eg] = evaluate_king(board, Color::White);

  mg += pawn_mg + rook_mg + bishop_mg + knight_mg + king_mg;
  eg += pawn_eg + rook_eg + bishop_eg + knight_eg + king_eg;

  int phase = get_game_phase(board);
  if (phase > 24)
    phase = 24;
  return (mg * phase + eg * (24 - phase)) / 24;
}

int Eval::black_eval(const Board &board) {
  int mg = 0;
  int eg = 0;
  int total_pawns = 0;

  for (Square sq = 0; sq < 64; ++sq) {
    Piece piece = board.get_piece(sq);
    if (piece == Piece::None)
      continue;

    if (piece_type(piece) == PieceType::Pawn)
      total_pawns++;

    if (piece_color(piece) != Color::Black)
      continue;

    int r = 7 - square_row(sq);
    int c = square_col(sq);

    switch (piece_type(piece)) {
    case PieceType::Pawn:
      mg += Middlegame::PawnValue(r, c);
      eg += Endgame::PawnValue(r, c);
      break;
    case PieceType::Knight:
      mg += Middlegame::KnightValue(r, c);
      eg += Endgame::KnightValue(r, c);
      break;
    case PieceType::Bishop:
      mg += Middlegame::BishopValue(r, c);
      eg += Endgame::BishopValue(r, c);
      break;
    case PieceType::Rook:
      mg += Middlegame::RookValue(r, c);
      eg += Endgame::RookValue(r, c);
      break;
    case PieceType::Queen:
      mg += Middlegame::QueenValue(r, c);
      eg += Endgame::QueenValue(r, c);
      break;
    case PieceType::King:
      mg += Middlegame::KingValue(r, c);
      eg += Endgame::KingValue(r, c);
      break;
    default:
      break;
    }
  }

  // Positional and structural terms
  auto [pawn_mg, pawn_eg] = evaluate_pawns(board, Color::Black);
  auto [rook_mg, rook_eg] = evaluate_rooks(board, Color::Black);
  auto [bishop_mg, bishop_eg] =
      evaluate_bishops(board, Color::Black, total_pawns);
  auto [knight_mg, knight_eg] =
      evaluate_knights(board, Color::Black, total_pawns);
  auto [king_mg, king_eg] = evaluate_king(board, Color::Black);

  mg += pawn_mg + rook_mg + bishop_mg + knight_mg + king_mg;
  eg += pawn_eg + rook_eg + bishop_eg + knight_eg + king_eg;

  int phase = get_game_phase(board);
  if (phase > 24)
    phase = 24;
  return (mg * phase + eg * (24 - phase)) / 24;
}

int Eval::evaluate(const Board &board) {
  int w = white_eval(board);
  int b = black_eval(board);
  return w - b;
}

} // namespace chess