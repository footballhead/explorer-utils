#include "room.h"

#include "file.h"

#include <cstring>

namespace {

constexpr auto kRoomRecordSize = 0x168;

constexpr auto kRoomTileOffset = 0x1;
constexpr auto kRoomObjectOffset = 0xA1;
constexpr auto kRoomMonsterIdOffset = 0x141;
constexpr auto kRoomMonsterCountOffset = 0x142;
constexpr auto kRoomNorthIdOffset = 0x143;
constexpr auto kRoomIdOffset = 0x149;

} // namespace

std::vector<room_t> LoadRooms(std::string const& filename)
{
    std::vector<room_t> builder;

    auto const data = ReadBinaryFile(filename);
    auto const num_rooms = data.size() / kRoomRecordSize;
    for (auto i = 0; i < num_rooms; ++i) {
        room_t room;

        auto const room_base = i * kRoomRecordSize;
        memcpy(room.tiles.data(), data.data() + room_base + kRoomTileOffset, kRoomArea);
        memcpy(room.objects.data(), data.data() + room_base + kRoomObjectOffset, kRoomArea);
        room.monster_id = data[room_base + kRoomMonsterIdOffset];
        room.monster_count = data[room_base + kRoomMonsterCountOffset];

        room.nav.north = data[room_base + kRoomNorthIdOffset];
        room.nav.east = data[room_base + kRoomNorthIdOffset + 1];
        room.nav.south = data[room_base + kRoomNorthIdOffset + 2];
        room.nav.west = data[room_base + kRoomNorthIdOffset + 3];
        room.nav.up = data[room_base + kRoomNorthIdOffset + 4];
        room.nav.down = data[room_base + kRoomNorthIdOffset + 5];

        room.id = data[room_base + kRoomIdOffset];

        builder.emplace_back(std::move(room));
    }

    // // TODO
    return builder;
}
