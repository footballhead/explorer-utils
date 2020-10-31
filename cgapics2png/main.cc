#include <explorer-utils/color.h>
#include <explorer-utils/file.h>

#include <stb_image_write/stb_image_write.h>

#include <array>
#include <vector>

namespace {

constexpr auto kImageWidth = 16;
constexpr auto kImageHeight = 16;
constexpr auto kImageArea = kImageWidth * kImageHeight;
constexpr auto kImageComponents = 3; // RGB

uint8_t HalfNibble(uint8_t const val, int const part)
{
    return (val >> (part*2)) & 0x3;
}

std::array<color_t, 4> cga = {
    color_t{0x00, 0x00, 0x00},
    color_t{0x00, 0xFF, 0xFF},
    color_t{0xFF, 0x00, 0xFF},
    color_t{0xFF, 0xFF, 0xFF},
};

std::vector<uint8_t> ToHalfNibbles(std::vector<uint8_t> const& data)
{
    std::vector<uint8_t> builder;
    builder.reserve(data.size() * 4);

    for (auto const& v : data) {
        builder.push_back(HalfNibble(v, 3));
        builder.push_back(HalfNibble(v, 2));
        builder.push_back(HalfNibble(v, 1));
        builder.push_back(HalfNibble(v, 0));
    }

    return builder;
}

std::vector<uint8_t> ThrowAwayGarbage(std::vector<uint8_t> const& data)
{
    std::vector<uint8_t> builder;

    // Of each 4-image-group, keep only the first image
    // For some reason, the back 3 are garbage...
    for (auto base = 0; base < data.size(); base += kImageArea * 4) {
        for (auto i = 0; i < kImageArea; ++i) {
            builder.push_back(data[i + base]);
        }
    }

    return builder;
}


} // namespace

int main()
{
    auto const cgapics = read_binary_file("CGAPICS.PIC");
    auto const half_nibbles = ThrowAwayGarbage(ToHalfNibbles(cgapics));

    std::vector<color_t> image;

    auto base = 0;
    for (base = 0; base < half_nibbles.size(); base += kImageArea) {
        for (auto y = 0; y < kImageHeight; ++y) {
            for (auto x = 0; x < kImageWidth; ++x) {
                // Throw away the first row because the images are actually 15x15 in 16x16 cells
                if (y % kImageWidth == 0) {
                    image.push_back(cga[0]);
                    continue;
                }
                image.push_back(cga[half_nibbles[base + y * kImageWidth + x]]);
            }
        }
    }

    // I'm relying on color_t to serialize properly without intervention
    stbi_write_png("CGAPICS.PNG", kImageWidth, base / kImageWidth, kImageComponents, image.data(), kImageWidth * kImageComponents);

    return 0;
}
