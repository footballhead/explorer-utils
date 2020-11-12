#include "monster.h"

#include "file.h"

namespace {
constexpr auto kMonDatRecordSize = 0x1F;
constexpr auto kMonDatGfxIdOffset = 0x16;
} // namespace

std::vector<monster_t> LoadMonsterData(std::string const& filename)
{
    auto const monster_data = ReadBinaryFile(filename);

    std::vector<monster_t> builder;
    for (int i = 0; i < monster_data.size() / kMonDatRecordSize; ++i) {
        auto const gfx = monster_data[i * kMonDatRecordSize + kMonDatGfxIdOffset];
        builder.push_back(monster_t{gfx});
    }
    return builder;
}