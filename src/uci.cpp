#include "core/Board.hpp"
#include "core/Eval.hpp"
#include "core/MoveGen.hpp"
#include "core/Search.hpp"
#include "core/Types.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string move_to_uci(const chess::Move &move) {
  if (!move.is_valid())
    return "0000";

  std::string uci = chess::square_to_algebraic(move.from) +
                    chess::square_to_algebraic(move.to);
  if (move.type == chess::MoveType::Promotion) {
    switch (move.promotion_piece) {
    case chess::PieceType::Queen:
      uci += 'q';
      break;
    case chess::PieceType::Rook:
      uci += 'r';
      break;
    case chess::PieceType::Bishop:
      uci += 'b';
      break;
    case chess::PieceType::Knight:
      uci += 'n';
      break;
    default:
      uci += 'q';
      break;
    }
  }
  return uci;
}

std::optional<chess::Move> parse_uci_move(chess::Board &board,
                                          const std::string &move_str) {
  if (move_str.length() < 4)
    return std::nullopt;

  auto from_sq = chess::square_from_algebraic(move_str.substr(0, 2));
  auto to_sq = chess::square_from_algebraic(move_str.substr(2, 2));
  if (!from_sq.has_value() || !to_sq.has_value())
    return std::nullopt;

  chess::PieceType promo = chess::PieceType::None;
  if (move_str.length() >= 5) {
    char promo_char = static_cast<char>(std::tolower(move_str[4]));
    switch (promo_char) {
    case 'q':
      promo = chess::PieceType::Queen;
      break;
    case 'r':
      promo = chess::PieceType::Rook;
      break;
    case 'b':
      promo = chess::PieceType::Bishop;
      break;
    case 'n':
      promo = chess::PieceType::Knight;
      break;
    default:
      break;
    }
  }

  auto legal_moves = chess::MoveGen::generate_legal_moves(board);
  for (const auto &m : legal_moves) {
    if (m.from == *from_sq && m.to == *to_sq) {
      if (m.type == chess::MoveType::Promotion) {
        if (m.promotion_piece == promo ||
            (promo == chess::PieceType::None &&
             m.promotion_piece == chess::PieceType::Queen)) {
          return m;
        }
      } else {
        return m;
      }
    }
  }
  return std::nullopt;
}

void print_board_ascii(const chess::Board &board) {
  std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
  for (int r = 0; r < 8; ++r) {
    std::cout << (8 - r) << " |";
    for (int c = 0; c < 8; ++c) {
      chess::Piece p = board.get_piece(r, c);
      char ch = chess::piece_to_char(p);
      std::cout << " " << (ch == ' ' ? '.' : ch) << " |";
    }
    std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
  }
  std::cout << "    a   b   c   d   e   f   g   h\n\n";
  std::cout << "FEN: " << board.to_fen() << "\n";
  std::cout << "Side to move: "
            << (board.side_to_move() == chess::Color::White ? "White" : "Black")
            << "\n\n";
}

} // anonymous namespace

int main() {
  // Disable input/output buffering for responsive UCI communication
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);

  chess::Board board;
  board.reset_to_starting_position();

  std::string line;
  while (std::getline(std::cin, line)) {
    // Trim leading and trailing whitespace
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    line.erase(line.find_last_not_of(" \t\r\n") + 1);
    if (line.empty())
      continue;

    std::istringstream iss(line);
    std::string command;
    iss >> command;

    if (command == "uci") {
      std::cout << "id name Chess-Engine-Cpp\n";
      std::cout << "id author Ved Vainateya\n";
      std::cout << "option name Threads type spin default 1 min 1 max 512\n";
      std::cout << "option name Hash type spin default 16 min 1 max 4096\n";
      std::cout << "option name Move Overhead type spin default 10 min 0 max 5000\n";
      std::cout << "option name Ponder type check default false\n";
      std::cout << "option name UCI_ShowWDL type check default false\n";
      std::cout << "option name SyzygyPath type string default <empty>\n";
      std::cout << "uciok\n" << std::endl;
    } else if (command == "isready") {
      std::cout << "readyok\n" << std::endl;
    } else if (command == "setoption") {
      std::string name_token, opt_name, value_token;
      if (iss >> name_token && name_token == "name") {
        std::string name;
        while (iss >> name && name != "value") {
          opt_name = name;
        }
        if (opt_name == "Hash") {
          int mb = 16;
          if (iss >> mb) {
            chess::Search::resize_tt(mb);
          }
        } else if (opt_name == "Threads") {
          int th = 1;
          iss >> th;
        }
      }
    } else if (command == "ucinewgame") {
      board.reset_to_starting_position();
      chess::Search::clear_tt();
    } else if (command == "position") {
      std::string subcmd;
      iss >> subcmd;

      if (subcmd == "startpos") {
        board.reset_to_starting_position();
        std::string moves_token;
        if (iss >> moves_token && moves_token == "moves") {
          std::string move_str;
          while (iss >> move_str) {
            auto move = parse_uci_move(board, move_str);
            if (move.has_value()) {
              board.make_move(*move);
            }
          }
        }
      } else if (subcmd == "fen") {
        std::string fen_part, fen;
        while (iss >> fen_part && fen_part != "moves") {
          if (!fen.empty())
            fen += " ";
          fen += fen_part;
        }
        board.load_fen(fen);

        // If there are moves after FEN
        if (fen_part == "moves") {
          std::string move_str;
          while (iss >> move_str) {
            auto move = parse_uci_move(board, move_str);
            if (move.has_value()) {
              board.make_move(*move);
            }
          }
        }
      }
    } else if (command == "go") {
      int depth = 64; // default max depth for iterative deepening
      int movetime = -1;
      int wtime = -1, btime = -1;
      int winc = 0, binc = 0;
      bool has_depth_limit = false;

      std::string token;
      while (iss >> token) {
        if (token == "depth") {
          iss >> depth;
          has_depth_limit = true;
        } else if (token == "movetime") {
          iss >> movetime;
        } else if (token == "wtime") {
          iss >> wtime;
        } else if (token == "btime") {
          iss >> btime;
        } else if (token == "winc") {
          iss >> winc;
        } else if (token == "binc") {
          iss >> binc;
        } else if (token == "infinite") {
          depth = 64;
          has_depth_limit = true;
        }
      }

      int time_limit_ms = -1;

      if (movetime > 0) {
        // Fixed time per move with 15ms communication safety buffer
        time_limit_ms = std::max(5, movetime - 15);
      } else {
        int my_time =
            (board.side_to_move() == chess::Color::White) ? wtime : btime;
        int my_inc =
            (board.side_to_move() == chess::Color::White) ? winc : binc;

        if (my_time > 0) {
          // Safe dynamic time management:
          // Budget ~1/25 of remaining clock + 70% increment
          int target_time = (my_time / 25) + (my_inc * 7 / 10);
          // Reserve 40ms safety buffer so we never flag on clock overrun
          int max_allowed = std::max(5, my_time - 40);
          time_limit_ms = std::clamp(target_time, 5, max_allowed);
        } else if (!has_depth_limit) {
          // No time and no depth provided: default to depth 5
          depth = 5;
        }
      }

      auto result = chess::Search::search(board, depth, time_limit_ms, true);
      if (result.best_move.is_valid()) {
        std::cout << "bestmove " << move_to_uci(result.best_move) << "\n"
                  << std::endl;
      } else {
        // If no legal move found, pick any legal move or none
        auto legals = chess::MoveGen::generate_legal_moves(board);
        if (!legals.empty()) {
          std::cout << "bestmove " << move_to_uci(legals.front()) << "\n"
                    << std::endl;
        } else {
          std::cout << "bestmove (none)\n" << std::endl;
        }
      }
    } else if (command == "stop") {
      // Currently search is synchronous; stop is a no-op if idle
    } else if (command == "d" || command == "print") {
      print_board_ascii(board);
    } else if (command == "eval") {
      int score = chess::Eval::evaluate(board);
      std::cout << "Static eval: " << score << " cp (White perspective)\n"
                << std::endl;
    } else if (command == "quit") {
      break;
    }
  }

  return 0;
}
