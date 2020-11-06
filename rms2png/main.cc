#include <explorer-utils/file.h>
#include <explorer-utils/image.h>
#include <explorer-utils/room.h>

#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {

constexpr auto kImageComponents = 3; // RGB

constexpr auto kMonDatRecordSize = 0x1F;
constexpr auto kMonDatGfxIdOffset = 0x16;

int GetObjectTile(uint8_t object)
{
    switch (object) {
    case 'd': // Magical darkness
        return 47;
    case 'e': // Treasure chest
        return 21;
    case 'f': // Smoke
        return 46;
    case 'g': // Movable block
        return 29;
    case 'h': // Door (vertical)
        return 37;
    case 'i': // Door (horizontal)
        return 36;
    case 'j': // Funny looking chest
        // TODO
        std::cerr << "WARN: Map wanted funny looking chest\n";
        break;
    case 'k': // Soft section of wall
        std::cerr << "WARN: Map wanted soft section of wall\n";
        // TODO
        break;
    case 'l': // Soft piece of wall
        return 2;
    case 'm': // Soft pile of rubble
        return 18;
    case 'n': // Old body
        return 22;
    case 'o': // Old bones
        return 17;
    case 'p': // Old stone coffin
        return 49;
    case 'q': // Old grave
        return 54;
    case 'r': // Movable glass block:
        // TODO
        std::cerr << "WARN: Map wanted movable glass block\n";
        break;
    case 's': // Old skeleton
        // TODO
        std::cerr << "WARN: Map wanted old skeleton (s)\n";
        break;
    case 't': // Old skeleton
        // TODO
        std::cerr << "WARN: Map wanted old skeleton (t)\n";
        break;
    case 'u': // Hollow obilisk
        // TODO
        std::cerr << "WARN: Map wanted Hollow obilisk\n";
        break;
    case 'v': // "Just some blood"
        return 82;
    case 'w': // Stone marker
        // TODO
        std::cerr << "WARN: wanted stone marker\n";
        break;
    }

    return 0;
}

int GetObjectTileMask(uint8_t object)
{
    switch (object) {
    case 'e': // Treasure chest
        return 64;
    case 'f': // Smoke
        return 66;
    case 'n': // Old body
        return 69;
    case 'o': // Old bones
        return 70;
    case 'p': // Old stone coffin
        return 71;
    case 'q': // Old grave
        return 65;
    }

    return 0;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " egapics.pic pymon.pic pymask.pic pymon.dat in.rms prefix\n";
        return 1;
    }

    auto const egapics_filename = argv[1];
    auto const pymon_filename = argv[2];
    auto const pymask_filename = argv[3];
    auto const pymon_dat_filename = argv[4];
    auto const rms_filename = argv[5];
    auto const out_prefix = argv[6];

    auto const tile_images = LoadEgaSpritesheet(egapics_filename);
    auto const monster_images = LoadEgaSpritesheet(pymon_filename);
    auto const monster_mask_images = LoadEgaSpritesheet(pymask_filename);
    auto const tile_width = tile_images[0].GetWidth();
    auto const tile_height = tile_images[0].GetHeight();

    auto const rooms = LoadRooms(rms_filename);
    auto const monster_data = ReadBinaryFile(pymon_dat_filename);

    for (auto room_index = 0; room_index < rooms.size(); ++room_index) {
        auto const& room = rooms[room_index];
        Image map_image(tile_width * kRoomWidth, tile_height * kRoomHeight);

        // TODO: Cheaters versions of the maps
        // Make glass walls visible
        // No magical darkness
        // Highlight important traps
        // Reveal soft walls and secret doors
        // No fog
        // Easier to see Quasits

        for (auto y = 0; y < kRoomHeight; ++y) {
            for (auto x = 0; x < kRoomWidth; ++x) {
                // FIRST do tile
                auto tile = room.tiles[y * kRoomWidth + x];
                
                // 0 is null, nothing here
                if (tile == 0) {
                    continue;
                }
                tile -= 1;

                // TODO: There are many kinds of traps (wands, XP) but they're stored as ASCII chars. Figure them out
                if (tile >= tile_images.size()) {
                    tile = 20;
                }

                auto const& tile_img = tile_images[tile];
                map_image.Blit(tile_img, x * tile_img.GetWidth(), y * tile_img.GetHeight());

                // SECOND do object
                auto object = room.objects[y * kRoomWidth + x];

                // 0 is null, nothing here
                if (object == 0) {
                    continue;
                }

                // Monsters (with mask)
                if (object <= 'c') {
                    auto const monster_index = room.monster_id - 1;
                    auto const monster_gfx = monster_data[monster_index * kMonDatRecordSize + kMonDatGfxIdOffset] - 1;
                    auto const& monster_img = monster_images[monster_gfx];
                    auto const& mask_img = monster_mask_images[monster_gfx];
                    map_image.Blit(monster_img, mask_img, x * monster_img.GetWidth(), y * monster_img.GetHeight());
                    continue;
                }

                tile = GetObjectTile(object);
                auto mask = GetObjectTileMask(object);

                // Tiles no mask
                if (mask == 0) {
                    auto const& obj_img = tile_images[tile];
                    map_image.Blit(obj_img, x * obj_img.GetWidth(), y * obj_img.GetHeight());
                    continue;
                }

                // Tiles with mask
                auto const& obj_img = tile_images[tile];
                auto const& mask_img = tile_images[mask];
                map_image.Blit(obj_img, mask_img, x * obj_img.GetWidth(), y * obj_img.GetHeight());
            }
        }

        // XXX: I'm relying on color_t to serialize properly without
        // intervention which may not be the case on non-x86 platforms
        auto const out_filename = out_prefix + std::to_string(room_index) + ".png";
        stbi_write_png(out_filename.c_str(), map_image.GetWidth(), map_image.GetHeight(), kImageComponents, map_image.GetData().data(), map_image.GetWidth() * kImageComponents);
    }

    return 0;
}
