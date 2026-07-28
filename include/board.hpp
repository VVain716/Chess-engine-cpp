#ifndef BOARD_HPP
#define BOARD_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "pixel.hpp"

bool drawBoard(SDL_Renderer *renderer, int width, int height);

#endif
