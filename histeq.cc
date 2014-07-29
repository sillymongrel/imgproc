#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "image.h"



/// Compute and return the histogram of a geryscale image. 
/// @param img      Input image pixels
/// @param w        Width of the input image
/// @param h        Height of the input image
/// @return Histogram of the image.
float* hist(const Image& img)
{
    float npix = img.width*img.height;
    float* ret = (float*)malloc(256 * sizeof(float));
    memset(ret, 0, 256*sizeof(float));
    for(int y = 0; y < img.height; ++y) {
        const uint8_t* scan = &img.pixels[y * img.width];
        for(int x = 0; x < img.width; ++x) {
            ret[scan[x]]++;
        }
    }
    for(int i = 0; i < 256; ++i) {
        ret[i] /= npix;
    }
    return ret;
}

/// Compute the histogram equalised image. Histogram equalisation is computed
/// using the following formula:
///
/// g_i = (M-1)/N SUM^i_{j=0} n_i
/// 
/// where:
/// M = 256 (total number of greylevels)
/// N = total nunber of pixels in the image
/// n_i is the number of pixels at greylevel i.
/// 
/// Using the CDF, the algorithm uniformly redistributes 
/// image greylevels so the probability of each greylevel 
/// is approximately the same. 
///
/// @param result       Output image buffer
/// @param img          Input image pixels
/// @param w            Width of the input image
/// @param h            Height of the input image
void histeq(Image& result, const Image& img)
{
    result.alloc(img.width, img.height);
    float* imgHist=hist(img);
    uint32_t* histEq = (uint32_t*)malloc(256*sizeof(uint32_t)); 
    memset(histEq, 0, 256*sizeof(uint32_t));

    // Compute CDF and rescale to 0-255 for each new greylevel.
    for(int i = 0; i < 256; ++i) {
        float sum = 0.0f;
        for(int j = 0; j <= i; ++j) {
            sum += imgHist[j];
        }
        histEq[i] = (int)(sum * 255.0f + 0.5f); // round
    }
    free(imgHist);

    // Apply the new greylevels to the output image
    for(int y = 0; y < img.height; ++y) {
        const uint8_t* scan = img.scanline(y);
        uint8_t* resScan = result.scanline(y);
        for(int x = 0; x < img.width; ++x) {
            resScan[x] = histEq[scan[x]];
        }
    }
    free(histEq);
}




int main(int argc, char** argv)
{
    if(argc < 2 || argc > 3) {
        printf("\n\nUsage: imgproc <imgfile> [outfile]\nDefault outfile is 'result.pgm'\n\n");
        return 1;
    }
    const char* outfile = (argc == 3) ? argv[2] : "result.pgm";
    Image img;
    if(readPGM(img, argv[1])) {
        Image result;
        histeq(result, img);
        writePGM(outfile, result);
    }

    return 0;
}



