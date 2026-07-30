#include "moves.hpp"


const std::vector<std::pair<int, int>> knight_moves = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}};
const std::vector<std::pair<int, int>> bishop_moves = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> rook_moves = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}};
const std::vector<std::pair<int, int>> queen_moves = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const std::vector<std::pair<int, int>> king_moves = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};


bool king_check(Board *board, int kingpos) {
  int king_row = kingpos / 8; int king_col = kingpos % 8;
  bool is_white = (board->get_piece(king_row*8+king_col) == WHITE_KING);
  
  // check whether a knight is attacking
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

  for (std::pair<int, int> move : bishop_moves) {
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
  
  // check whether we are hit by a rook or a queen
  for (std::pair<int, int> move : rook_moves) {
    int curr_row = king_row; int curr_col = king_col;
    while (1){ 
      curr_row += move.first;
      curr_col += move.second;

      if (curr_row < 0 || curr_row > 7 || curr_col < 0 || curr_col > 7) {
        break; // no rook or queen has been seen before we have hit the end of the board
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
  bool is_white = (piece <= 6);


  if (is_white) {
    // pawn
    if (piece == WHITE_PAWN) {
      // check if the pawn can move two squares
      if (curr_row == 0) {return res;}
      else if (curr_row == 6 && board->get_piece((curr_row - 2)*8+curr_col) == EMPTY && board->get_piece((curr_row - 1)*8+curr_col) == EMPTY) {
        res.push_back((curr_row - 2)*8+curr_col);
        res.push_back((curr_row-1)*8+curr_col);
      }
      // move up if there is no piece 
      else if (curr_row - 1 >= 0 && board->get_piece((curr_row - 1)*8+curr_col) == EMPTY) {
        res.push_back((curr_row - 1)*8+curr_col);
      }
      
      // handle cases where the pawn can capture
      if (curr_col - 1 >= 0 && board->get_piece((curr_row-1)*8+curr_col-1) > WHITE_KING) {
        res.push_back((curr_row-1)*8+curr_col-1);
      }
      if (curr_col + 1 < 8 && board->get_piece((curr_row-1)*8+curr_col+1) > WHITE_KING) {
        res.push_back((curr_row-1)*8+curr_col+1);
      }
    }
    // knight
    else if (piece == WHITE_KNIGHT) {
      for (std::pair move : knight_moves) {
        int row = move.first + curr_row;
        int col = move.second + curr_col;

        // check if out of bounds
        if (row < 0 || row > 7 || col < 0 || col > 7) {continue;}

        if (board->get_piece(row*8+col) == EMPTY || board->get_piece(row*8+col) > WHITE_KING) {
          res.push_back(row*8+col);
        }
      }
    }

    // bishop
    else if (piece == WHITE_BISHOP) {
      for (std::pair<int, int> move : bishop_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            break;
          }

          // only other case: black piece
          else {
            res.push_back(row*8+col);
            break;
          }
        }
      }
    }


    // rook -- very similar to bishop, just different set of moves
    else if (piece == WHITE_ROOK) {
      for (std::pair<int, int> move : rook_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            break;
          }

          // only other case: black piece
          else {
            res.push_back(row*8+col);
            break;
          }
        }
      }
    }
    // queen -- very similar to rook and bishop 
    else if (piece == WHITE_QUEEN) {
      for (std::pair<int, int> move : queen_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            break;
          }

          // only other case: black piece
          else {
            res.push_back(row*8+col);
            break;
          }
        }
      }
    }
      // king -- similar to the knights movement 
    else if (piece == WHITE_KING) {
      for (std::pair move : king_moves) {
        int row = move.first + curr_row;
        int col = move.second + curr_col;
        
        // check if out of bounds
        if (row < 0 || row > 7 || col < 0 || col > 7) {continue;}

        if (board->get_piece(row*8+col) == EMPTY || board->get_piece(row*8+col) > WHITE_KING) {
          res.push_back(row*8+col);
        }
      }
    }
  }










  // black
  else {
    // pawn
    if (piece == BLACK_PAWN) {
      // check if the pawn can move two squares
      if (curr_row == 7) {return res;}
      else if (curr_row == 1 && board->get_piece((curr_row + 2)*8+curr_col) == EMPTY && board->get_piece((curr_row + 1)*8+curr_col) == EMPTY) {
        res.push_back((curr_row + 2)*8+curr_col);
        res.push_back((curr_row+1)*8+curr_col);
      }
      // move up if there is no piece 
      else if (curr_row + 1 < 8 && board->get_piece((curr_row + 1)*8+curr_col) == EMPTY) {
        res.push_back((curr_row + 1)*8+curr_col);
      }
      
      // handle cases where the pawn can capture
      if (curr_col + 1 >= 0 && board->get_piece((curr_row+1)*8+curr_col-1) <= WHITE_KING && board->get_piece((curr_row+1)*8+curr_col-1)) {
        res.push_back((curr_row+1)*8+curr_col-1);
      }
      if (curr_col + 1 < 8 && board->get_piece((curr_row+1)*8+curr_col+1) <= WHITE_KING && board->get_piece((curr_row+1)*8+curr_col+1)) {
        res.push_back((curr_row+1)*8+curr_col+1);
      }
    }
    // knight
    else if (piece == BLACK_KNIGHT) {
      for (std::pair move : knight_moves) {
        int row = move.first + curr_row;
        int col = move.second + curr_col;

        // check if out of bounds
        if (row < 0 || row > 7 || col < 0 || col > 7) {continue;}

        if (board->get_piece(row*8+col) <= WHITE_KING) {
          res.push_back(row*8+col);
        }
      }
    }

    // bishop
    else if (piece == BLACK_BISHOP) {
      for (std::pair<int, int> move : bishop_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            res.push_back(row*8+col);
            break;
          }

          // only other case: black piece
          else {
            break;
          }
        }
      }
    }


    // rook -- very similar to bishop, just different set of moves
    else if (piece == BLACK_ROOK) {
      for (std::pair<int, int> move : rook_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            res.push_back(row*8+col);
            break;
          }

          // only other case: black piece
          else {
            break;
          }
        }
      }
    }
    // queen -- very similar to rook and bishop 
    else if (piece == BLACK_QUEEN) {
      for (std::pair<int, int> move : queen_moves) {
        int row = curr_row; int col = curr_col;
        while (1) {
          row += move.first;
          col += move.second;
          int piece = board->get_piece(row*8+col);
          // edge of the board
          if (row < 0 || row > 7 || col < 0 || col > 7) {break;}
          
          // empty square
          else if (piece == EMPTY) {
            res.push_back(row*8+col);
          }

          // white piece
          else if (piece != EMPTY && piece <= WHITE_KING) {
            res.push_back(row*8+col);
            break;
          }

          // only other case: black piece
          else {
            break;
          }
        }
      }
    }
      // king -- similar to the knights movement 
    else if (piece == BLACK_KING) {
      for (std::pair<int, int> move : king_moves) {
        int row = move.first + curr_row;
        int col = move.second + curr_col;
        
        // check if out of bounds
        if (row < 0 || row > 7 || col < 0 || col > 7) {continue;}

        if (board->get_piece(row*8+col) <= WHITE_KING) {
          res.push_back(row*8+col);
        }
      }
    }
  }
  //TODO: Handle castling and en-passent 

  std::vector<int> without_checks;
  for (int move : res) {
    // store the previous pieces 
    int src = board->get_piece(pos);
    int dst = board->get_piece(move);
    
    // simulate the move
    board->set_piece(pos, EMPTY);
    board->set_piece(move, src);
    
    if (board->get_piece(move) == WHITE_KING) {
      white_king = move;
    }
    else if (board->get_piece(move) == BLACK_KING) {
      black_king = move;
    }

    if (is_white && !king_check(board, white_king)) {
      without_checks.push_back(move);
    }
    
    else if (!is_white && !king_check(board, black_king)) {
      without_checks.push_back(move);
    }

    // reset to previous positions
    board->set_piece(pos, src);
    board->set_piece(move, dst);
  }
  return without_checks;
}
