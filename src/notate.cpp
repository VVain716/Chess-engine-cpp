#include "notate.hpp"

std::string Coordinate::get_moves() {
  return s;
}

std::string Coordinate::notate_pos(int pos) {
  int row = pos / 8;
  int col = pos % 8;

  char x = (7 - row) + '0';
  char y = col + 'a';
  std::string res = "";
  return res + y + x;;
}

bool Coordinate::add_move(Board *board, int src, int dest) {
  int piece_val = board->get_piece(src);
  if (piece_val == EMPTY) {
    return false;
  }

  std::string src_notation = notate_pos(src);
  std::string dest_notation = notate_pos(dest);
  std::string piece = "";
  switch (piece_val) {
    case WHITE_KNIGHT:
    case BLACK_KNIGHT:
      piece += 'N';
      break;
    case WHITE_ROOK:
    case BLACK_ROOK:
      piece += 'R';
      break;
    case WHITE_BISHOP:
    case BLACK_BISHOP:
      piece += 'B';
      break;
    case WHITE_QUEEN:
    case BLACK_QUEEN:
      piece += 'Q';
      break;
    case WHITE_KING:
    case BLACK_KING:
      piece += 'K';
      break;
    default:
      break;
  }  

  char transition;
  if (board->get_piece(dest) != EMPTY) {
    transition = 'x';
  }
  else {
    transition = '-';
  }

  std::string res = "";
  res += piece + src_notation + transition + dest_notation;

  if (get_color(piece_val)) {
    s += std::to_string(move_number) + "." + res + " ";
  }
  else {
    s += res + " ";
    move_number++;
  }
  return true;
}


bool Coordinate::add_kingside(Board *board, bool is_white) {
  if (is_white) {
    s += std::to_string(move_number) + ".0-0 ";
  }
  else {
    s += "0-0 ";
    move_number++;
  }
  return true;
}

bool Coordinate::add_queenside(Board *board, bool is_white) {
  if (is_white) {
    s += std::to_string(move_number) + ".0-0-0 ";
  }
  else {
    s += "0-0-0 ";
    move_number++;
  }
  return true;
}


bool Coordinate::black_win() {
  s += ("\n" RED "0-1" ESCAPE);
  return true;
}

bool Coordinate::white_win() {
  s += ("\n" GREEN "1-0" ESCAPE);
  return true;
}

bool Coordinate::draw() {
  s += ("\n" YELLOW "0.5-0.5" ESCAPE);
  return true;
}
