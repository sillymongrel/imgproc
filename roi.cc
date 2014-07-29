#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <vector>


#include "image.h"
#include "filter.h"
#include "qtree.h"

struct SplitPred
{
    bool operator()(const Image& img, QTreeNode<float>& node)
    {
        if(node.size() < 50) {
            return false;
        }
        float avg = 0;
        for(int y = node.top; y < node.bottom(); ++y)
        {
            const uint8_t* line = img.scanline(y);        
            for(int x = node.left; x < node.right(); ++x)
            {
                avg += line[x];
            }        
        }
        avg /= (float)node.size();
        node.data = avg;
        for(int y = node.top; y < node.bottom(); ++y)
        {
            const uint8_t* line = img.scanline(y);        
            for(int x = node.left; x < node.right(); ++x)
            {
                if(abs(line[x] - avg) > 20) {
                    return true;
                }
            }        
        }
        return false; // All pixels with 30 of the average
    }
};

struct IterFunc
{
    Image* image;
    const Image* source;
    IterFunc(Image* img, const Image* src) :
        image(img), source(src)
    {
    }
    bool operator()(QTreeNode<float>& node)
    {
        uint8_t* lineTop = image->scanline(node.top);
        uint8_t* lineBottom = image->scanline(node.bottom()-1);
        for(int x = node.left; x < node.right(); ++x)
        {
            lineTop[x] = 255;
            lineBottom[x] = 255;
        }

        for(int y = node.top; y < node.bottom(); ++y)
        {
            image->pixels[node.left + y * image->width] = 255;
            image->pixels[node.right()-1 + y * image->width] = 255;
        }
        return true;
    }
};

void quadtreeTest(const Image& img)
{
    Image test = img;
    // Split on average pixel value to find regions of similar grey level.
    QTree<float> qt;
    SplitPred splitem;
    qt.make(test, splitem);

    IterFunc iterfunc(&test, &img);
    qt.foreach(iterfunc);

    writePGM("qtreetest.pgm", test);
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
AABB findStrip(const Image& img)
{
    SearchState state;
    state.ctx = SEARCH_FOR_BLACK;
    state.pixCount = 0;
    
    Image sharp = Image(img.width, img.height);
    sharpen(sharp, img);
    average(sharp, sharp);
    Image test = Image(img.width, img.height);
    writePGM("sharpen.pgm", sharp);

    
    // Hold all X and Y coordinates for the min
    std::vector<int> minX;
    minX.reserve(sharp.height);
    
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
                minX.push_back(x);
                break;
            }
            if(x > img.width / 3) {
                break;
            }
        }
    }
    writePGM("test.pgm", test);
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
    aabb.set(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    const char* outfile = (argc == 7) ? argv[6] : "result.pgm";

    Image img;
    readPGM(img, infile);

    Image result;
    crop(result, img, aabb);    

    writePGM(outfile, result);

    quadtreeTest(img);

    findStrip(img);
}

