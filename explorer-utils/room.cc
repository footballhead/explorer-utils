#include "room.h"

#include <cstring>
#include <iostream>

#include "file.h"

namespace {

constexpr auto kRoomRecordSize = 0x168;

constexpr auto kRoomTileOffset = 0x1;
constexpr auto kRoomObjectOffset = 0xA1;
constexpr auto kRoomMonsterIdOffset = 0x141;
constexpr auto kRoomMonsterCountOffset = 0x142;
constexpr auto kRoomNorthIdOffset = 0x143;
constexpr auto kRoomIdOffset = 0x149;

}  // namespace

uint8_t room_t::GetTile(int x, int y) const {
  auto const tile = tiles[y * kRoomWidth + x];
  // Different traps are different ASCII characters
  if (tile > 84) {
    return 21;
  }
  return tile;
}

auto room_t::GetObjectType(int x, int y) const -> object_type {
  auto const tile = objects[y * kRoomWidth + x];
  if (tile == 0) {
    return object_type::none;
  }
  if (tile <= 'c') {
    return object_type::monster;
  }
  return object_type::object;
}

uint8_t room_t::GetObject(int x, int y) const {
  auto const tile = objects[y * kRoomWidth + x];
  return GetObjectTile(tile);
}

int GetObjectTile(uint8_t object) {
  switch (object) {
    case 'd':  // Magical darkness
      return 47;
    case 'e':  // Treasure chest
      return 21;
    case 'f':  // Smoke
      return 46;
    case 'g':  // Movable block
      return 29;
    case 'h':  // Door (vertical)
      return 37;
    case 'i':  // Door (horizontal)
      return 36;
    case 'j':  // Funny looking chest
      return 21;
    case 'k':  // Soft section of wall
      return 2;
    case 'l':  // Soft piece of wall
      return 42;
    case 'm':  // Soft pile of rubble
      return 18;
    case 'n':  // Old body
      return 22;
    case 'o':  // Old bones
      return 17;
    case 'p':  // Old stone coffin
      return 49;
    case 'q':  // Old grave
      return 54;
    case 'r':  // Movable glass block:
      return 0;
    case 's':  // Old skeleton
      return 74;
    case 't':  // Old skeleton
      return 75;
    case 'u':  // Hollow obilisk
      return 79;
    case 'v':  // "Just some blood"
      return 82;
    case 'w':  // Stone marker
      return 59;
  }

  return 0;
}

int GetObjectTileMask(uint8_t object) {
  switch (object) {
    case 'e':  // Treasure chest
      return 64;
    case 'f':  // Smoke
      return 66;
    case 'n':  // Old body
      return 69;
    case 'o':  // Old bones
      return 70;
    case 'p':  // Old stone coffin
      return 71;
    case 'q':  // Old grave
      return 65;
    case 's':  // Old skeleton
      return 72;
    case 't':  // Old skeleton
      return 73;
    case 'v':  // Just some blood
      return 83;
    case 'w':  // Stone marker
      return 81;
  }

  return 0;
}

std::vector<room_t> LoadRooms(std::string const& filename) {
  std::vector<room_t> builder;

  auto const data = ReadBinaryFile(filename);
  auto const num_rooms = data.size() / kRoomRecordSize;
  for (auto i = 0; i < num_rooms; ++i) {
    room_t room;

    auto const room_base = i * kRoomRecordSize;
    memcpy(room.tiles.data(), data.data() + room_base + kRoomTileOffset,
           kRoomArea);
    memcpy(room.objects.data(), data.data() + room_base + kRoomObjectOffset,
           kRoomArea);
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
