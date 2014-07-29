#ifndef imageH
#define imageH

#include <stdint.h>
#include <algorithm>
#include <vector>


struct Image
{
    std::vector<uint8_t> pixels;
    int width,height;

    Image();
    Image(int w, int h);
    ~Image();

    void alloc(int w, int h);
    void release();

    // Get the scanline at row Y
    uint8_t* scanline(int y);
    const uint8_t* scanline(int y) const;

    int size() const
    {
        return this->width*this->height;
    }    

};

struct AABB
{
private:
    int _xmin, _ymin;
    int _xmax, _ymax;
public:
    AABB() {}
    AABB(int minx, int miny, int maxx, int maxy)
    {
        this->set(minx, miny, maxx, maxy);
    }
    void set(int minx, int miny, int maxx, int maxy)
    {
        _xmin = std::min(minx, maxx);
        _xmax = std::max(minx, maxx);
        _ymin = std::min(miny, maxy);
        _ymax = std::max(miny, maxy);
    }
    bool isWithin(const Image& img) const;
    int xmin() const  {return _xmin;}
    int xmax() const  {return _xmax;}
    int ymin() const  {return _ymin;}
    int ymax() const  {return _ymax;}
    int width() const  {return _xmax-_xmin;}
    int height() const  {return _ymax-_ymin;}

};

Image readPGM(const char* fname);
void writePGM(const char* fname, const Image& img);

#endif //imageH
