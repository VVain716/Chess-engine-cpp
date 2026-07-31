#ifndef MOVES_HPP
#define MOVES_HPP

#include "board.hpp"
#include "animate.hpp"

bool king_check(Board *board, int kingpos); // kingpos is an integer between 0 and 63
std::vector<int> get_legal_moves(int pos, Board *board, int white_king, int black_king);
bool get_color(int piece); // returns true when the color is white, otherwise returns false. does not account for EMPTY
std::vector<std::pair<int, int>> total_legal_moves(Board *board, bool is_white, int white_king, int black_king); // function that returns {src, dest} pairs for all the possible legal moves 
bool checkmate(Board *board, bool is_white, int white_king, int black_king);
bool stalemate(Board *board, bool is_white, int white_king, int black_king);


// helper functions to determine whether white/black can castle kingside or queenside
namespace CheckCastle {
    bool white_kingside_castle(Board *board);
    bool black_kingside_castle(Board *board);
    bool white_queenside_castle(Board *board);
    bool black_queenside_castle(Board *board);
}

namespace ExecuteCastle {
    bool white_kingside_castle(SDL_Renderer *renderer, Board *board, int width, int height);
    bool black_kingside_castle(SDL_Renderer *renderer, Board *board, int width, int height);
    bool white_queenside_castle(SDL_Renderer *renderer, Board *board, int width, int height);
    bool black_queenside_castle(SDL_Renderer *renderer, Board *board, int width, int height);
}


#endif
