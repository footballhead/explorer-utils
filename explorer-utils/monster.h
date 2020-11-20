#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct monster_t {
  uint8_t gfx;
};

std::vector<monster_t> LoadMonsterData(
    std::string const& filename = "PYMON.DAT");
