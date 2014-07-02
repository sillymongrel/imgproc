#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


const int WIDTH = 240;
const int HEIGHT = 320;



uint8_t* readPGM(const char* fname)
{
    FILE* fptr = fopen(fname, "rb");
    if(!fptr) {
        printf("failed to read file '%s'\n", fname);
        return NULL;
    }
    fseek(fptr, 0, SEEK_END);
    int flen = ftell(fptr);
    fseek(fptr, 0xF, SEEK_SET);// Skip the image header.

    uint8_t* ret = (uint8_t*)malloc(flen+1*sizeof(uint8_t));
    fread(ret, flen, 1, fptr); 
    fclose(fptr);
    return ret;
}

void writePGM(const char* fname, const uint8_t* img, const int w, const int h)
{
    FILE* fptr = fopen(fname, "wb");
    const char outdat[] = "P5 240 320 255 ";
    fwrite(outdat, sizeof(char), sizeof(outdat), fptr);
    fwrite(img, sizeof(uint8_t), w*h, fptr);
    fclose(fptr);
}
/// Compute and return the histogram of a geryscale image. 
float* hist(const uint8_t* img, const int w, const int h) 
{
    float* ret = (float*)malloc(256 * sizeof(float));
    memset(ret, 0, 256*sizeof(float));
    for(int y = 0; y < h; ++y) {
        const uint8_t* scan = &img[y * w];
        for(int x = 0; x < w; ++x) {
            ret[scan[x]]++;
        }
    }
    for(int i = 0; i < 256; ++i) {
        ret[i] /= (float)(w*h);
    }
    return ret;
}

// Compute the histogram equalised image. Histogram equalisation is computed
// using the following formula:
//
// g_i = (M-1)/N SUM^i_{j=0} n_i
// 
// where:
// M = 256 (total number of greylevels)
// N = total nunber of pixels in the image
// n_i is the number of pixels at greylevel i.
// 
// This formula normalises the CDF of the histogram to the 
// 
// for each pixel level. 
void histeq(uint8_t* result, const uint8_t* img, const int w, const int h)
{
    float* imgHist=hist(img, w, h);
    uint32_t* histEq = (uint32_t*)malloc(256*sizeof(uint32_t)); 
    memset(histEq, 0, 256*sizeof(uint32_t));
    for(int i = 0; i < 256; ++i) {
        float sum = 0.0f;
        for(int j = 0; j <= i; ++j) {
            sum += imgHist[j];
        }
        histEq[i] = (int)(sum * 255.0f + 0.5f); // round
    }
    free(imgHist);
    for(int y = 0; y < h; ++y) {
        const uint8_t* scan = &img[y*w];
        uint8_t* resScan = &result[y*w];
        for(int x = 0; x < w; ++x) {
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
    uint8_t* img = readPGM(argv[1]);
    if(img) {
        uint8_t* result = (uint8_t*)malloc(WIDTH*HEIGHT*sizeof(uint8_t));
        histeq(result, img, WIDTH, HEIGHT);
        writePGM(outfile, result, WIDTH, HEIGHT);
        free(img);
        free(result);
    }

    return 0;
}



