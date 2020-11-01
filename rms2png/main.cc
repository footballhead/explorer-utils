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
    auto const monster_images = LoadEgaSpritesheet(pymon_filename);
    auto const tile_width = tile_images[0].GetWidth();
    auto const tile_height = tile_images[0].GetHeight();

    auto rms_data = ReadBinaryFile(rms_filename);

    for (auto map_index = 0; map_index < rms_data.size() / kRoomRecordSize; ++map_index) {
        Image map_image(tile_width * kMapWidth, tile_height * kMapHeight);

        for (auto y = 0; y < kMapHeight; ++y) {
            for (auto x = 0; x < kMapWidth; ++x) {
                // FIRST do tile
                auto tile = rms_data[(map_index * kRoomRecordSize) + kRoomTileOffset + y * kMapWidth + x] - 1;

                // HACK: Only trap tiles seem to be out of bounds
                if (tile >= tile_images.size()) {
                    tile = 20;
                }

                auto const& tile_img = tile_images[tile];
                map_image.Blit(tile_img, x * tile_img.GetWidth(), y * tile_img.GetHeight());

                // SECOND do object
                // TODO: Object mask/transparency
                auto object = rms_data[(map_index * kRoomRecordSize) + kRoomMonsterOffset + y * kMapWidth + x];

                // tile 0 is the null tile, no monster here
                if (object == 0) {
                    continue;
                }

                if (object <= 'c') {
                    // TODO why are monsters wrong?
                    continue;
                }

                switch (object) {
                case 'd': // Magical darkness
                    tile = 12;
                    break;
                case 'e': // Treasure chest
                    tile = 21;
                    break;
                case 'f': // Smoke
                    tile = 46;
                    break;
                case 'g': // Movable block
                    tile = 29;
                    break;
                case 'h': // Door (vertical)
                    tile = 37;
                    break;
                case 'i': // Door (horizontal)
                    tile = 36;
                    break;
                case 'j': // Funny looking chest
                    // TODO
                    std::cerr << "WARN: Map " << map_index << " wanted funny looking chest\n";
                    break;
                case 'k': // Soft section of wall
                    std::cerr << "WARN: Map " << map_index << " wanted soft section of wall\n";
                    // TODO
                    break;
                case 'l': // Soft piece of wall
                    // TODO
                    std::cerr << "WARN: Map " << map_index << " wanted soft piece of wall\n";
                    break;
                case 'm': // Soft pile of rubble
                    tile = 18;
                    break;
                case 'n': // Old body
                    tile = 22;
                    break;
                case 'o': // Old bones
                    tile = 17;
                    break;
                case 'p': // Old stone coffin
                    tile = 49;
                    break;
                case 'q': // Old grave
                    tile = 54;
                    break;
                case 'r': // Movable glass block:
                    // TODO
                    std::cerr << "WARN: Map " << map_index << " wanted movable glass block\n";
                    break;
                case 's': // Old skeleton
                    tile = 74;
                    break;
                case 't': // Old skeleton
                    tile = 75;
                    break;
                case 'u': // Hollow obilisk
                    // TODO
                    std::cerr << "WARN: Map " << map_index << " wanted hollo obilisk\n";
                    break;
                case 'v': // "Just some blood"
                    tile = 82;
                    break;
                case 'w': // Stone marker
                    // TODO
                    std::cerr << "WARN: Map " << map_index << " wanted stone marker\n";
                    break;
                default:
                    tile = object - 1;
                    break;
                }

                auto const& obj_img = object <= 'c' ? monster_images[tile] : tile_images[tile];
                map_image.Blit(obj_img, x * obj_img.GetWidth(), y * obj_img.GetHeight());
            }
        }

        // XXX: I'm relying on color_t to serialize properly without
        // intervention which may not be the case on non-x86 platforms
        auto const out_filename = out_prefix + std::to_string(map_index) + ".png";
        stbi_write_png(out_filename.c_str(), map_image.GetWidth(), map_image.GetHeight(), kImageComponents, map_image.GetData().data(), map_image.GetWidth() * kImageComponents);
    }

    return 0;
}
