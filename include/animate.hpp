#ifndef ANIMATE_HPP
#define ANIMATE_HPP

#ifndef FPS
#define FPS 60
#endif

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "pieces.hpp"
#include "board.hpp"

bool animate_move(SDL_Renderer *renderer, int src, int dest, Board *board, int width, int height);

// src and dest are both from 0 to 63 inclusive


#endif
