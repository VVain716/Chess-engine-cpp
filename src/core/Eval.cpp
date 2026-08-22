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
// Phase & General Evaluation
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

int Eval::white_eval(const Board &board) {
  int mg = 0;
  int eg = 0;
  for (Square sq = 0; sq < 64; ++sq) {
    Piece piece = board.get_piece(sq);
    if (piece == Piece::None || piece_color(piece) != Color::White)
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
  int phase = get_game_phase(board);
  if (phase > 24)
    phase = 24;
  return (mg * phase + eg * (24 - phase)) / 24;
}

int Eval::black_eval(const Board &board) {
  int mg = 0;
  int eg = 0;
  for (Square sq = 0; sq < 64; ++sq) {
    Piece piece = board.get_piece(sq);
    if (piece == Piece::None || piece_color(piece) != Color::Black)
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