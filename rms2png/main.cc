#include <explorer-utils/file.h>
#include <explorer-utils/image.h>

#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 3; // RGB
constexpr auto kMapWidth = 20;
constexpr auto kMapHeight = 8;

constexpr auto kRoomRecordSize = 0x168;
constexpr auto kRoomTileOffset = 0x1;
constexpr auto kRoomMonsterOffset = 0xA1;

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " egapics.pic pymon.pic in.rms prefix\n";
        return 1;
    }

    auto const egapics_filename = argv[1];
    auto const pymon_filename = argv[2];
    auto const rms_filename = argv[3];
    auto const out_prefix = argv[4];

    auto const tile_images = LoadEgaSpritesheet(egapics_filename);
    // auto const monster_images = LoadEgaSpritesheet(pymon_filename);
    auto const tile_width = tile_images[0].GetWidth();
    auto const tile_height = tile_images[0].GetHeight();

    auto rms_data = ReadBinaryFile(rms_filename);

    for (auto map_index = 0; map_index < rms_data.size() / kRoomRecordSize; ++map_index) {
        Image map_image(tile_width * kMapWidth, tile_height * kMapHeight);

        for (auto y = 0; y < kMapHeight; ++y) {
            for (auto x = 0; x < kMapWidth; ++x) {
                auto tile = rms_data[(map_index * kRoomRecordSize) + kRoomTileOffset + y * kMapWidth + x] - 1;

                // HACK: Only trap tiles seem to be out of bounds
                if (tile >= tile_images.size()) {
                    tile = 20;
                }

                auto const& img = tile_images[tile];
                map_image.Blit(img, x * img.GetWidth(), y * img.GetHeight());
            }
        }

        // for (auto y = 0; y < kMapHeight; ++y) {
        //     for (auto x = 0; x < kMapWidth; ++x) {
        //         auto tile = rms_data[(map_index * kRoomRecordSize) + kRoomMonsterOffset + y * kMapWidth + x];

        //         // tile 0 is the null tile, no monster here
        //         if (tile == 0 || tile >= monster_images.size()) {
        //             continue;
        //         }
        //         tile -= 1;

        //         auto const& img = monster_images[tile];
        //         map_image.Blit(img, x * img.GetWidth(), y * img.GetHeight());
        //     }
        // }

        // XXX: I'm relying on color_t to serialize properly without
        // intervention which may not be the case on non-x86 platforms
        auto const out_filename = out_prefix + std::to_string(map_index) + ".png";
        stbi_write_png(out_filename.c_str(), map_image.GetWidth(), map_image.GetHeight(), kImageComponents, map_image.GetData().data(), map_image.GetWidth() * kImageComponents);
    }

    return 0;
}
