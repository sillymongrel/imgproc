#include <stdint.h>
#include <assert.h>
#include "image.h"

const int sharpMask[] = {-1, -1, -1,
                         -1, 9, -1,
                         -1, -1, -1};
const float sharpScale = 1.0f;
const int avgMask[] = {1, 1, 1,
                         1, 1, 1,
                         1, 1, 1};
const float avgScale = 0.1111;

const int embossMask[] = {-2, 1, 0,
                      -1, 1, 1,
                      0, 1, 2};
const float embossScale = 0.33;

// a simple repeat at border 3x3 convolution
uint8_t convolve3x3(const Image& imgIn, int x, int y, const int* mask, float scale)
{
    float newpix = 0.0;
    // 3x3, just do it manually...
    int minY = (y-1 >= 0) ? y-1 : 0;
    int maxY = (y+1 < imgIn.height) ? y+1 : imgIn.height-1;
    
    int minX = (x-1 >= 0) ? x-1 : 0;
    int maxX = (x+1 < imgIn.width) ? x+1 : imgIn.width-1;

    // top row
    const uint8_t* row = imgIn.scanline(minY);
    newpix  +=  mask[0] * row[minX];
    newpix  +=  mask[1] * row[x];
    newpix  +=  mask[2] * row[maxX];

    // middle row
    row = imgIn.scanline(y);
    newpix  +=  mask[3] * row[minX];
    newpix  +=  mask[4] * row[x];
    newpix  +=  mask[5] * row[maxX];

    // Bottom row
    row = imgIn.scanline(maxY);
    newpix  +=  mask[6] * row[minX];
    newpix  +=  mask[7] * row[x];
    newpix  +=  mask[8] * row[maxX];

    // Scale the result accordingly and clamp to a valid pixel range.
    newpix *= scale;
    if(newpix > 255.0) {
        newpix = 255.0;
    } else if(newpix < 0) {
        newpix = 0;
    }
    return (uint8_t)newpix;
}



void emboss(Image& ret, const Image& img)
{
    assert(ret.size() == img.size() && "image dimensions do not match");
    for(int y = 0; y < img.height; ++y)
    {
        uint8_t* retline = ret.scanline(y);
        for(int x = 0; x < img.width; ++x)
        {
            retline[x] = convolve3x3(img, x, y, embossMask, embossScale);
        }
    }
}

void average(Image& ret, const Image& img)
{
    assert(ret.size() == img.size() && "image dimensions do not match");
    for(int y = 0; y < img.height; ++y)
    {
        uint8_t* retline = ret.scanline(y);

        for(int x = 0; x < img.width; ++x)
        {
            retline[x] = convolve3x3(img, x, y, avgMask, avgScale);
        }
    }
}
void sharpen(Image& ret, const Image& img)
{
    assert(ret.size() == img.size() && "image dimensions do not match");
    for(int y = 0; y < img.height; ++y)
    {
        uint8_t* retline = ret.scanline(y);
        for(int x = 0; x < img.width; ++x)
        {
            retline[x] = convolve3x3(img, x, y, sharpMask, sharpScale);
        }
    }
}
