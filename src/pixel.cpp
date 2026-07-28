#include "pixel.hpp"

Pixel newPixel(int red, int green, int blue, int alpha) {
  Pixel x; 
  x.red = red; x.green = green; x.blue = blue; x.alpha = alpha;
  return x;
}   
