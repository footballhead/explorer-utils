#pragma once

#include <cstdint>

struct color_t {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

constexpr auto color_white = color_t{0xFF, 0xFF, 0xFF};
constexpr auto color_black = color_t{0x00, 0x00, 0x00};

constexpr inline bool operator==(color_t const& a, color_t const& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
