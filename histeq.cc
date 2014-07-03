#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>





/// Compute and return the histogram of a geryscale image. 
/// @param img      Input image pixels
/// @param w        Width of the input image
/// @param h        Height of the input image
/// @return Histogram of the image.
float* hist(const uint8_t* img, const int w, const int h) 
{
    float npix = w*h;
    float* ret = (float*)malloc(256 * sizeof(float));
    memset(ret, 0, 256*sizeof(float));
    for(int y = 0; y < h; ++y) {
        const uint8_t* scan = &img[y * w];
        for(int x = 0; x < w; ++x) {
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
void histeq(uint8_t* result, const uint8_t* img, const int w, const int h)
{
    float* imgHist=hist(img, w, h);
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
    for(int y = 0; y < h; ++y) {
        const uint8_t* scan = &img[y*w];
        uint8_t* resScan = &result[y*w];
        for(int x = 0; x < w; ++x) {
            resScan[x] = histEq[scan[x]];
        }
    }
    free(histEq);
}


uint8_t* readPGM(const char* fname, int& w, int& h)
{
    FILE* fptr = fopen(fname, "r");
    char dummy[256];
    if(!fptr) {
        printf("failed to read file '%s'\n", fname);
        return NULL;
    }
    // Skip the image header.
    fgets(dummy, 255, fptr);
    if(dummy[0] != 'P' && dummy[1] != '5') {
        printf("image not recognised\n");
        exit(1);
    }
    fgets(dummy, 256, fptr);
    if(dummy[0] == '#') {
        fgets(dummy, 256, fptr);
    }

    sscanf(dummy, "%d %d", &w, &h);
    fgets(dummy, 256, fptr);

    printf("Image:%s\nWidth:%d\nHeight:%d\n", fname, w, h);

    uint8_t* ret = (uint8_t*)malloc(w*h*sizeof(uint8_t));
    fread(ret, w*h, 1, fptr); 
    fclose(fptr);
    return ret;
}

void writePGM(const char* fname, const uint8_t* img, const int w, const int h)
{
    FILE* fptr = fopen(fname, "wb");

    char outdat[256];
    sprintf(outdat, "P5\n# Equalised image\n%d %d\n255\n", w, h);
    fprintf(fptr, "%s", outdat); //outdat, sizeof(char), sizeof(outdat), fptr);
    fwrite(img, sizeof(uint8_t), w*h, fptr);
    fclose(fptr);
}


int main(int argc, char** argv)
{
    if(argc < 2 || argc > 3) {
        printf("\n\nUsage: imgproc <imgfile> [outfile]\nDefault outfile is 'result.pgm'\n\n");
        return 1;
    }
    const char* outfile = (argc == 3) ? argv[2] : "result.pgm";
    int w, h;
    uint8_t* img = readPGM(argv[1], w, h);
    if(img) {
        uint8_t* result = (uint8_t*)malloc(w*h*sizeof(uint8_t));
        histeq(result, img, w, h);
        writePGM(outfile, result, w, h);
        free(img);
        free(result);
    }

    return 0;
}



