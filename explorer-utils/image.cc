#include "image.h"
#include "file.h"

#include <array>

void Image::Blit(Image const& src, int x, int y)
{
    for (auto yy = 0; yy < src.GetHeight(); ++yy) {
        for (auto xx = 0; xx < src.GetWidth(); ++xx) {
            Set(xx + x, yy + y, src.Get(xx, yy));
        }
    }
}

void Image::Blit(Image const& src, Image const& mask, int x, int y)
{
    for (auto yy = 0; yy < src.GetHeight(); ++yy) {
        for (auto xx = 0; xx < src.GetWidth(); ++xx) {
            if (mask.Get(xx, yy) == color_black) {
                Set(xx + x, yy + y, src.Get(xx, yy));
            }
        }
    }
}
