
#include <stdio.h>
#include <stdlib.h>

#include "image.h"

Image emptyImage()
{
    Image ret;
    ret.pixels = NULL;
    ret.width = ret.height = -1;
    return ret;
}

Image allocImage(int w, int h)
{
    Image ret;
    ret.pixels = (uint8_t*)malloc(w*h * sizeof(uint8_t));
    ret.width = w;
    ret.height = h;
    return ret;
}

void freeImage(Image* img)
{
    free(img->pixels);
    img->pixels = NULL;
    img->width = img->height = -1;
}


Image readPGM(const char* fname)
{
    FILE* fptr = fopen(fname, "r");
    char dummy[256];
    if(!fptr) {
        printf("failed to read file '%s'\n", fname);
        return emptyImage();
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

    Image ret = allocImage(w, h);
    fread(ret.pixels, ret.width * ret.height, 1, fptr); 
    fclose(fptr);
    return ret;
}

void writePGM(const char* fname, const Image img)
{
    FILE* fptr = fopen(fname, "wb");

    char outdat[256];
    sprintf(outdat, "P5\n# Equalised image\n%d %d\n255\n", img.width, img.height);
    fprintf(fptr, "%s", outdat); //outdat, sizeof(char), sizeof(outdat), fptr);
    fwrite(img.pixels, sizeof(uint8_t), img.width*img.height, fptr);
    fclose(fptr);
}

