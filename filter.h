#ifndef filterH
#define filterH

#include <stdint.h>
#include "image.h"

// a simple repeat at border 3x3 convolution
uint8_t convolve3x3(const Image& imgIn, int x, int y, const int* mask, float scale);
void sharpen(Image& ret, const Image& img);
void emboss(Image& ret, const Image& img);
void average(Image& ret, const Image& img);

#endif // filterH
