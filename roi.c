#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "image.h"
#include "filter.h"





/// Crop a rectangular region of the input image and return
/// as a new image. The crop is inclusive of the endX, endY.
Image crop(const Image img, 
            const AABB aabb)
{
    // Sanity check 
    
    assert(aabb.xmax > aabb.xmin && "aabb.xmax must be greater than aabb.xmin");
    assert(aabb.ymax > aabb.ymin && "aabb.ymax must be greater than aabb.ymin");
    assert(aabb.xmin >= 0 && "aabb.xmin must be greater than 0");
    assert(aabb.ymin >= 0 && "aabb.ymin must be greater than 0");
    assert(aabb.xmax < img.width && "aabb.xmax must be less than the image width");
    assert(aabb.ymax < img.height && "aabb.ymax must be less than the image height");

    Image ret = allocImage(aabb.xmax-aabb.xmin+1, aabb.ymax-aabb.ymin+1);
    for(int y = aabb.ymin, y1 = 0; y <= aabb.ymax; ++y, ++y1)
    {
        uint8_t* inLine = &img.pixels[y * img.width + aabb.xmin];
        memcpy(&ret.pixels[y1 * ret.width], inLine, ret.width * sizeof(uint8_t));
    }
    return ret;
}


// Search for the strip.
// 
// This is a first pass dump of ideas. Scan from the image edge
// to find the strip edge. This is not straight forward because there
// are false edges in the surrounding pixels.
//
// It works as follows:
// 1. Threshold the image into black, white and grey
// 
// 2. Scan until a run of N black pixels is found, followed by a 
// run of M white pixels.
//
// 3. If a grey pixel is found, flag it but continue the scan. If
// more than O grey pixels are seen, reset the scan count.
//
// 4. Only scan the third of the image that is interesting
//
// 5. Compute the average X for the vertical edges and average
// Y for the horizontal edges.

// There are better ways to do this but they require a little more
// time and effort, i.e. compute the edge response and given the
// surrounding pixels (flow direction) classify the pixel as 
// strip or non-strip edge.
typedef struct _SearchState
{
    int ctx;
    int pixCount;
    int hmmCount; // number of pixels we're not sure about.

} SearchState;

#define SEARCH_FOR_BLACK 0
#define SEARCH_FOR_WHITE 1
#define FOUND_TRANSITION 2
#define HMM_NOT_SURE 3

    // MAGIC values, need to fix these
static const int PIXEL_THRESH_MIN = 80; 
static const int PIXEL_THRESH_MAX = 140;
static const int MIN_BLACK = 5; // min consecutive black pixels before we accept this is a possible edge
static const int MIN_WHITE = 3; // min consecutive white pixels before we accept this is a possible transition
static const int MAX_NOT_SURE = 2; // Number of not sure pixels before we reset back to search_black state

/// Update the given state for a given pixel value. This assumes the
/// pixel has been threshold to be 0 or 255.
static void updateState(SearchState* state, uint8_t pixel)
{
    switch(state->ctx)
    {
        case SEARCH_FOR_BLACK:
        {
            if(pixel == 0)
            {
                ++state->pixCount;
            }
            if(state->pixCount >= MIN_BLACK)
            {
                state->pixCount = 0;
                state->ctx = SEARCH_FOR_WHITE;
            }
            break;
        }
        case HMM_NOT_SURE:
        {
            if(pixel == 128) 
            {
                state->hmmCount++;
                if(state->hmmCount > MAX_NOT_SURE) {
                    state->ctx = SEARCH_FOR_BLACK;
                }                
            }
            else if(pixel == 0) {
                state->hmmCount = 0;
                state->ctx = SEARCH_FOR_BLACK;
            }
            else if(pixel == 255) {
                state->hmmCount = 0;
                state->ctx = SEARCH_FOR_WHITE;
            }
            break;
        }
        case SEARCH_FOR_WHITE:
        {
            if(pixel == 255)
            {
                ++state->pixCount;
            }
            else if(pixel == 128)
            {
                    
                // We'll include this pixel but set the state to not sure in case
                // it doesn't give us a valid WHITE run
                state->ctx=HMM_NOT_SURE;
            }
            if(state->pixCount > MIN_WHITE)
            {
                state->ctx = FOUND_TRANSITION;
                state->pixCount = 0;
            }
            break;
        }
        case FOUND_TRANSITION:
        {
            break;

        }
    }

}

// Search for the strip bar.
// This is a very SIMPLE scan for the strip. It looks for a run of 
// black pixels then finds the transition
AABB findStrip(const Image img)
{
    SearchState state;
    state.ctx = SEARCH_FOR_BLACK;
    state.pixCount = 0;
    
    Image sharp = sharpen(img);
    sharp = average(sharp);
    Image test = allocImage(img.width, img.height);
    writePGM("sharpen.pgm", sharp);

    memset(test.pixels, 0, (img.width*img.height)*sizeof(uint8_t));

    // Hold all X and Y coordinates for the min
    uint8_t* minX = (uint8_t*)malloc(sharp.height * sizeof(uint8_t));
    
    int minXidx = 0;
    // Scan each row
    for(int y = 0; y < sharp.height; ++y)
    {
        // Each new line we reset the context to search for
        // black strip
        state.ctx = SEARCH_FOR_BLACK;
        state.pixCount = 0;
        uint8_t* line = &sharp.pixels[y * sharp.width];       
        uint8_t* tmpLine = &test.pixels[y * sharp.width];
        for(int x = 0; x < sharp.width; ++x)
        {            
            uint8_t tpix = line[x];
            if(tpix < PIXEL_THRESH_MIN) {
                tpix = 0;
            } else if(tpix > PIXEL_THRESH_MAX) {
                tpix = 255;
            } else {
                tpix = 128;
            }
            updateState(&state, tpix);

            if(state.ctx == FOUND_TRANSITION) 
            {
                tmpLine[x] = 255;
                minX[minXidx++] = x;
                break;
            }
            if(x > img.width / 3) {
                break;
            }
        }
    }
    writePGM("test.pgm", test);
    freeImage(&test);
    AABB ret;
    return ret;
}

int main(int argc, char** argv)
{
    if(argc < 6 || argc > 7) 
    {
        printf("Usage: roi <infile> <xmin> <ymin> <xmax> <ymax> [outfile]\n\n\tIf outfile is not specified 'result.pgm' will be used.\n\n");
        return -1;
    }

    char* infile = argv[1];
    AABB aabb;
    aabb.xmin = atoi(argv[2]);
    aabb.ymin = atoi(argv[3]);
    aabb.xmax = atoi(argv[4]);
    aabb.ymax = atoi(argv[5]);

    const char* outfile = (argc == 7) ? argv[6] : "result.pgm";

    Image img = readPGM(infile);
    Image result = crop(img, aabb);    
    writePGM(outfile, result);

    findStrip(img);

    freeImage(&img);
    freeImage(&result);
}

