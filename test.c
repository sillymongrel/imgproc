#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef struct _Image
{
    uint8_t* data;
    int w,h;
} Image;

Image f(int w, int h)
{
    Image ret;
    ret.data = (uint8_t*)malloc(w*h * sizeof(uint8_t));
    ret.w = w;
    ret.h = h;
    return ret;

}


int main()
{
    Image img = f(320, 240);
    printf("data:%p, w:%d, h:%d", img.data, img.w, img.h);

    return 0;
}
