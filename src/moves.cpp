#include "moves.hpp"


bool king_check(Board *board, int kingpos) {
  int king_row = kingpos / 8; int king_col = kingpos % 8;
  bool is_white = (board->get_piece(king_row*8+king_col) == WHITE_KING);
  
  // check whether a knight is attacking
  std::vector<std::pair<int, int>> knight_moves = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}};
  for (std::pair<int, int> x : knight_moves) {
    int curr_row = king_row + x.first;
    int curr_col = king_col + x.second;
    if (curr_row < 0 || curr_row > 7 || curr_col < 0 || curr_col > 7) {continue;}
    int piece = board->get_piece(curr_row * 8 + curr_col);
    if (is_white && piece == BLACK_KNIGHT) {
      return true;
    }
    else if (!is_white && piece == WHITE_KNIGHT) {
      return true;
    }
  }

  // check whether pawns are attacking
  if (is_white) {
    // pawns will be attacking from king_row - 1, king_col +- 1
    if (king_row - 1 >= 0 && king_col - 1 >= 0 && board->get_piece((king_row - 1) * 8 + king_col - 1) == BLACK_PAWN) {
      return true;
    }
    else if (king_row - 1 >= 0 && king_col + 1 < 8 && board->get_piece((king_row - 1) * 8 + king_col + 1) == BLACK_PAWN) {
      return true;
    }
  } 
  else {
    // pawns will be attacking from king_row + 1, king_col +- 1
    if (king_row + 1 < 8 && king_col - 1 >= 0 && board->get_piece((king_row + 1) * 8 + king_col - 1) == WHITE_PAWN) {
      return true;
    }
    else if (king_row + 1 < 8 && king_col + 1 < 8 && board->get_piece((king_row + 1) * 8 + king_col + 1) == WHITE_PAWN) {
      return true;
    }
  }

  // check bishops and queens
  std::vector<std::pair<int, int>> diagonal_moves = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (std::pair<int, int> move : diagonal_moves) {
    int curr_row = king_row; int curr_col = king_col;
    while (1){
      curr_row += move.first;
      curr_col += move.second;
      if (curr_row < 0 || curr_row > 7 || curr_col < 0 || curr_col > 7) {
        break; // no bishop or queen has been seen before we have hit the end of the board
      }
      // check if we have hit a piece
      int piece = board->get_piece(curr_row * 8 + curr_col);
      if (is_white && (piece == BLACK_QUEEN || piece == BLACK_BISHOP)) {
        return true;
      }
      else if (!is_white && (piece == WHITE_QUEEN || piece == WHITE_BISHOP)) {
        return true;
      }
      else if (piece != EMPTY) {
        break; // we have seen a piece that isn't a threat, we can safely break
      }
    }
  }
  
  // check whether we are hit by a rook or a king
  std::vector<std::pair<int, int>> orthogonal_moves = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}};
  for (std::pair<int, int> move : orthogonal_moves) {
    int curr_row = king_row; int curr_col = king_col;
    while (1){ 
      curr_row += move.first;
      curr_col += move.second;

      if (curr_row < 0 || curr_row > 7 || curr_col < 0 || curr_col > 7) {
        break; // no bishop or queen has been seen before we have hit the end of the board
      }
      
      // check if we have hit a piece
      int piece = board->get_piece(curr_row * 8 + curr_col);
      if (is_white && (piece == BLACK_QUEEN || piece == BLACK_ROOK)) {
        return true;
      }
      else if (!is_white && (piece == WHITE_QUEEN || piece == WHITE_ROOK)) {
        return true;
      }
      else if (piece != EMPTY) {
        break;
      }
    }
  }

  // check adjacent moves to see if we are hit by the king 
  for (int drow = -1; drow <= 1; drow++) {
    for (int dcol = -1; dcol <= 1; dcol++) {
      if (drow == 0 && dcol == 0) {continue;}
      int curr_row = king_row + drow;
      int curr_col = king_col + dcol;
      if (curr_row > 7 || curr_row < 0 || curr_col > 7 || curr_col < 0) {
        continue;
      }
      if (is_white && board->get_piece(curr_row * 8 + curr_col) == BLACK_KING) {
        return true;
      } 
      else if (!is_white && board->get_piece(curr_row * 8 + curr_col) == WHITE_KING) {
        return true;
      }
    }
  }
  return false;
}


std::vector<int> get_legal_moves(int pos, Board *board, int white_king, int black_king) {
  std::vector<int> res;
  int curr_row = pos / 8;
  int curr_col = pos % 8;
  int piece = board->get_piece(pos);
  if (piece == 0) {
    return res;
  }
  else if (piece <= 6) {
    if (piece == WHITE_PAWN) {
      // for now, ignore the two square constraint
      
      // move up if there is no piece 
      if (curr_row - 1 >= 0 && board->get_piece((curr_row - 1)*8+curr_col) == EMPTY) {
        res.push_back((curr_row - 1)*8+curr_col);
      }
    }
  }

  else {

  }
  return res;
}
