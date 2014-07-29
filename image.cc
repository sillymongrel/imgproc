
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <string.h>

#include "image.h"

Image::Image() 
{
    this->width = this->height = -1;
}
Image::Image(int w, int h)
{
    this->alloc(w, h);
}

Image::~Image()
{
    this->release();
}

void Image::alloc(int w, int h)
{
    this->pixels = std::vector<uint8_t>(w*h); // Fills with 0
    this->width = w;
    this->height = h;
}

void Image::release()
{
    this->pixels.clear();
    this->width = this->height = -1;
}

// Get the scanline at row Y
uint8_t* Image::scanline(int y)
{
    if(this->pixels.size() > (unsigned)y * this->width) {
        return &this->pixels[y*this->width];
    } else {
        return NULL;
    }
}
const uint8_t* Image::scanline(int y) const
{
    if(this->pixels.size() > (unsigned)y * this->width) {
        return &this->pixels[y*this->width];
    } else {
        return NULL;
    }
}

bool readPGM(Image& ret, const char* fname)
{
    FILE* fptr = fopen(fname, "r");
    char dummy[256];
    if(!fptr) {
        printf("failed to read file '%s'\n", fname);
        return false;
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

    int w, h;
    sscanf(dummy, "%d %d", &w, &h);
    fgets(dummy, 256, fptr);

    printf("Image:%s\nWidth:%d\nHeight:%d\n", fname, w, h);

    ret.alloc(w, h);
    if(!ret.isValid()) {
        return false;
    }
    fread(&ret.pixels[0], ret.width * ret.height, 1, fptr); 
    fclose(fptr);
    return true;
}

bool writePGM(const char* fname, const Image& img)
{
    FILE* fptr = fopen(fname, "wb");
    if(!fptr) {
        return false;
    }
    char outdat[256];
    sprintf(outdat, "P5\n# Equalised image\n%d %d\n255\n", img.width, img.height);
    fprintf(fptr, "%s", outdat); //outdat, sizeof(char), sizeof(outdat), fptr);
    fwrite(&img.pixels[0], sizeof(uint8_t), img.size(), fptr);
    fclose(fptr);
    return true;
}

bool AABB::isWithin(const Image& img) const
{
    return (this->_xmin >= 0) && (this->_ymin >= 0) &&
           (this->_xmax < img.width) && (this->_ymax < img.height);
}

/// Crop a rectangular region of the input image and return
/// as a new image. The crop is inclusive of the endX, endY.
void crop(Image& ret,
            const Image& img, 
            const AABB& aabb)
{
    // Sanity check 
    assert(aabb.isWithin(img) && "AABB is outside image bounds");

    ret = Image(aabb.width()+1, aabb.height()+1);
    for(int y = aabb.ymin(), y1 = 0; y <= aabb.ymax(); ++y, ++y1)
    {
        const uint8_t* inLine = &img.pixels[y * img.width + aabb.xmin()];
        memcpy(ret.scanline(y1), inLine, ret.width * sizeof(uint8_t));
    }
}
