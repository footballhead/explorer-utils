#include "image.h"
#include "file.h"

#include <array>

namespace {

constexpr auto kImageWidth = 15;
constexpr auto kImageHeight = 15;

constexpr auto kCellWidth = 16;
constexpr auto kCellHeight = 16;
constexpr auto kCellArea = kCellWidth * kCellHeight;

constexpr auto kBitsPerByte = 8;
constexpr auto kHalfNibbleBits = 2;
constexpr auto kNumHalfNibblesPerByte = kBitsPerByte / kHalfNibbleBits;

constexpr auto kCgaBitsPerPixel = 2; // 4-color CGA half-nibble
constexpr auto kCgaPixelsPerByte = kBitsPerByte / kCgaBitsPerPixel;
constexpr auto kCgaBytesPerRow = kCellWidth / kCgaPixelsPerByte;
constexpr auto kCgaCellSizeInBytes = kCellHeight * kCgaBytesPerRow;

// In the CGA format, the 4 factor here really has no meaning. It's just that
// the format wastes 3/4 of the file. Since EGA uses these extra bytes, my
// guess is that this is a convenience to make CGA and EGA share alignment.
constexpr auto kEgaBitsPerPixel = 4; // 16-color EGA nibble
constexpr auto kEgaBytesPerRow = kCgaBytesPerRow * kEgaBitsPerPixel;
constexpr auto kImageAlignmentInBytes = kCgaCellSizeInBytes * kEgaBitsPerPixel;

auto constexpr cga_palette = std::array<color_t, 4>{
    color_t{0x00, 0x00, 0x00},
    color_t{0x00, 0xFF, 0xFF},
    color_t{0xFF, 0x00, 0xFF},
    color_t{0xFF, 0xFF, 0xFF},
};

auto constexpr ega_palette = std::array<color_t, 16>{
    color_t{0x00, 0x00, 0x00},
    color_t{0x00, 0x00, 0xAA},
    color_t{0x00, 0xAA, 0x00},
    color_t{0x00, 0xAA, 0xAA},
    color_t{0xAA, 0x00, 0x00},
    color_t{0xAA, 0x00, 0xAA},
    color_t{0xAA, 0x55, 0x00},
    color_t{0xAA, 0xAA, 0xAA},
    color_t{0x55, 0x55, 0x55},
    color_t{0x55, 0x55, 0xFF},
    color_t{0x55, 0xFF, 0x55},
    color_t{0x55, 0xFF, 0xFF},
    color_t{0xFF, 0x55, 0x55},
    color_t{0xFF, 0x55, 0xFF},
    color_t{0xFF, 0xFF, 0x55},
    color_t{0xFF, 0xFF, 0xFF}
};

/// Helper method to extract half-nibbles from a buffered byte.
inline constexpr uint8_t HalfNibble(uint8_t const val, int const part)
{
    return (val >> (part*2)) & 0x3;
}

} // namespace

std::vector<Image> LoadCgaSpritesheet(std::string const& filename)
{
    auto const cga_data = ReadBinaryFile(filename);

    auto const num_images = cga_data.size() / kImageAlignmentInBytes;
    std::vector<Image> images(num_images, Image(kImageWidth, kImageHeight));

    for (auto image_index = 0; image_index < num_images; ++image_index) {
        auto& image = images[image_index];
        auto const image_offset = image_index * kImageAlignmentInBytes;

        // Ignore first full row (contains garbage)
        for (auto y = 1; y < kCellHeight; ++y) {
            for (auto x = 0; x < kCgaBytesPerRow; ++x) {
                auto const byte_offset = image_offset + (y * kCgaBytesPerRow) + x;
                auto const b = cga_data[byte_offset];

                // (y - 1) to compensate for ignoring the first row
                image.Set(x * kCgaPixelsPerByte, y - 1, cga_palette[HalfNibble(b, 3)]);
                image.Set(x * kCgaPixelsPerByte + 1, y - 1, cga_palette[HalfNibble(b, 2)]);
                image.Set(x * kCgaPixelsPerByte + 2, y - 1, cga_palette[HalfNibble(b, 1)]);

                // Last byte of row only contains 3 pixels (because final image
                // is 15 wide, not 16 wide)
                if (x != kCgaBytesPerRow - 1) {
                    image.Set(x * kCgaPixelsPerByte + 3, y - 1, cga_palette[HalfNibble(b, 0)]);
                }
            }
        }
    }

    return images;
}

std::vector<Image> LoadEgaSpritesheet(std::string const& filename)
{
    auto const ega_data = ReadBinaryFile(filename);

    auto const num_images = ega_data.size() / kImageAlignmentInBytes;
    std::vector<Image> images(num_images, Image(kImageWidth, kImageHeight)); // TODO 15x15

    for (auto image_index = 0; image_index < num_images; ++image_index) {
        auto& image = images[image_index];
        auto const image_offset = image_index * kImageAlignmentInBytes;

        for (auto y = 0; y < kCellHeight - 1; ++y) {
            for (auto byte_column = 0; byte_column < kCgaBytesPerRow; ++byte_column) {
                auto const byte_offset = image_offset + (y * kEgaBytesPerRow) + byte_column;
                auto const next_byte_offset = byte_offset + kEgaBytesPerRow;

                for (auto halfnibble = 3; halfnibble >= 0; --halfnibble) {
                    // + kCgaBytesPerRow * X    to "stride" rows
                    // HalfNibble               to address a specific pixel
                    // / 2                      to convert data half-nibble to bit
                    // << Y                     to place bit in proper EGA index
                    auto const ega =
                        ((HalfNibble(ega_data[byte_offset + kCgaBytesPerRow * 1], halfnibble) / 2) << 0x3) |
                        ((HalfNibble(ega_data[byte_offset + kCgaBytesPerRow * 2], halfnibble) / 2) << 0x2) |
                        ((HalfNibble(ega_data[byte_offset + kCgaBytesPerRow * 3], halfnibble) / 2) << 0x1) |
                         (HalfNibble(ega_data[next_byte_offset],                  halfnibble) / 2);

                    // Last byte of row only contains 3 pixels (because final image
                    // is 15 wide, not 16 wide)
                    auto const x = byte_column * kCgaBytesPerRow + (3 - halfnibble);
                    if (x < kImageWidth) {
                        image.Set(x, y, ega_palette[ega]);
                    }
                }
            }
        }
    }

    return images;
}
