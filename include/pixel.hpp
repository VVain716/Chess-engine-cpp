#ifndef PIXEL_HPP
#define PIXEL_HPP

typedef struct Pixel {
  int red;
  int green;
  int blue;
  int alpha;
}Pixel;

Pixel newPixel(int red, int blue, int green, int alpha);


#endif
