#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

constexpr auto kRoomWidth = 20;
constexpr auto kRoomHeight = 8;
constexpr auto kRoomArea = kRoomWidth * kRoomHeight;

struct room_t {
  // uint8_t unknown;
  // Tiles are an index into EGAPICS/CGAPIECS
  std::array<uint8_t, kRoomArea> tiles;
  // Objects can be one of two things:
  // - An monster (see `monster_id`) (if < 'd')
  // - An index into EGAPICS/CGAPIECS (if >= 'd')
  std::array<uint8_t, kRoomArea> objects;
  // An index into PYMON.DAT, used for objects < 'd'
  uint8_t monster_id;
  uint8_t monster_count;
  struct {
    uint8_t north;
    uint8_t east;
    uint8_t south;
    uint8_t west;
    uint8_t up;
    uint8_t down;
  } nav;
  uint8_t id;
  // uint8_t unknown;
  // uint8_t unknown;
  // uint8_t unknown;
  // std::string name;

  enum class object_type { none, monster, object };

  uint8_t GetTile(int x, int y) const;
  object_type GetObjectType(int x, int y) const;
  uint8_t GetObject(int x, int y) const;
};

std::vector<room_t> LoadRooms(std::string const& filename);
