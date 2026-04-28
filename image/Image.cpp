#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.hpp"
#include "../utilities/RGBColor.hpp"
#include "../world/ViewPlane.hpp"
#include "stb_image_write.h"
#include <algorithm>

Image::Image(int h, int v) : hres(h), vres(v)
{
    colors = new RGBColor *[hres];
    for (int i = 0; i < hres; i++)
        colors[i] = new RGBColor[vres];
}

Image::Image(const ViewPlane &vp) : Image(vp.hres, vp.vres) {}

Image::~Image()
{
    for (int i = 0; i < hres; i++)
        delete[] colors[i];
    delete[] colors;
}

void Image::set_pixel(int x, int y, const RGBColor &color)
{
    colors[x][y] = color;
}

void Image::write_png(std::string path) const
{
    unsigned char *data = new unsigned char[hres * vres * 3];
    for (int y = 0; y < vres; y++)
    {
        for (int x = 0; x < hres; x++)
        {
            int idx = (y * hres + x) * 3;
            data[idx + 0] = (unsigned char)(std::min(colors[x][y].r, 1.0f) * 255.0f);
            data[idx + 1] = (unsigned char)(std::min(colors[x][y].g, 1.0f) * 255.0f);
            data[idx + 2] = (unsigned char)(std::min(colors[x][y].b, 1.0f) * 255.0f);
        }
    }
    stbi_write_png(path.c_str(), hres, vres, 3, data, hres * 3);
    delete[] data;
}
