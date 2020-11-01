#include <explorer-utils/color.h>
#include <explorer-utils/file.h>
#include <explorer-utils/palette.h>

#include <stb_image_write/stb_image_write.h>

#include <array>
#include <iostream>
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

std::vector<uint8_t> ToBits(std::vector<uint8_t> const& data)
{
    std::vector<uint8_t> builder;
    builder.reserve(data.size() * 4);

    for (auto const& v : data) {
        builder.push_back(HalfNibble(v, 3) / 2);
        builder.push_back(HalfNibble(v, 2) / 2);
        builder.push_back(HalfNibble(v, 1) / 2);
        builder.push_back(HalfNibble(v, 0) / 2);
    }

    return builder;
}

std::vector<uint8_t> CombineColumns(std::vector<uint8_t> const& data)
{
    std::vector<uint8_t> builder;
    builder.reserve(data.size() / 4);

    for (auto base = 0; base < data.size(); base += kImageArea * 4) {
        for (auto row = 0; row < kImageHeight; ++row) {
            auto row_index = row * kImageWidth * 4;
            auto next_row_index = ((row + 1) * kImageWidth * 4) % (kImageArea * 4);

            for (auto column = 0; column < kImageWidth; ++column) {
                builder.push_back(
                    (data[base + row_index + column + kImageWidth*3] << 0x1) |
                    (data[base + row_index + column + kImageWidth*2] << 0x2) |
                    (data[base + row_index + column + kImageWidth*1] << 0x3) |
                    data[base + next_row_index + column]
                );
            }
        }
    }

    return builder;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " in.pic out.png\n";
        return 1;
    }

    auto const in_filename = argv[1];
    auto const out_filename = argv[2];

    auto const egapics = read_binary_file(in_filename);
    auto const bits = ToBits(egapics);
    auto const combined = CombineColumns(bits);

    std::vector<color_t> image;

    auto base = 0;
    for (base = 0; base < combined.size(); base += kImageArea) {
        for (auto y = 0; y < kImageHeight; ++y) {
            for (auto x = 0; x < kImageWidth; ++x) {
                // Throw away the last row because the images are actually 15x15 in 16x16 cells
                if (y % kImageWidth == 15) {
                    image.push_back(ega_palette[0]);
                    continue;
                }

                image.push_back(ega_palette[combined[base + y * kImageHeight + x]]);
            }
        }
    }

    // I'm relying on color_t to serialize properly without intervention
    stbi_write_png(out_filename, kImageWidth, base / kImageWidth, kImageComponents, image.data(), kImageWidth * kImageComponents);

    return 0;
}
