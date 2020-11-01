#pragma once

#include "color.h"

#include <string>
#include <vector>

class Image {
public:
    Image(int width, int height)
    : width_(width)
    , height_(height)
    , pixels_(width * height, color_t{0, 0, 0})
    {
    }

    color_t Get(int x, int y) const { return pixels_.at(y * width_ + x); }
    void Set(int x, int y, color_t const& c) { pixels_[y * width_ + x] = c; }

    void Blit(Image const& src, int x, int y);

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    std::vector<color_t> const& GetData() const { return pixels_; }

private:
    int width_;
    int height_;
    std::vector<color_t> pixels_;
};

std::vector<Image> LoadCgaSpritesheet(std::string const& filename);
std::vector<Image> LoadEgaSpritesheet(std::string const& filename);
