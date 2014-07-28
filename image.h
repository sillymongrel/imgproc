#ifndef imageH
#define imageH

#include <stdint.h>

typedef struct _Image
{
    uint8_t* pixels;
    int width,height;
} Image;

Image emptyImage();

Image allocImage(int w, int h);

void freeImage(Image* img);
Image readPGM(const char* fname);
void writePGM(const char* fname, const Image img);


typedef struct _AABB
{
    int xmin, ymin;
    int xmax, ymax;

} AABB;


#endif //imageH
