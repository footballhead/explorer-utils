#include <explorer-utils/monster.h>
#include <explorer-utils/spritesheet.h>
#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 4;  // RGBA
}  // namespace

int main(int argc, char** argv) {
  if (argc)

    if (argc < 5) {
      std::cerr << "Usage: " << argv[0]
                << " pymon.pic pymask.pic pymon.dat out.png\n";
      return 1;
    }

  auto const pymon_pic_filename = argv[1];
  auto const pymask_pic_filename = argv[2];
  auto const pymon_dat_filename = argv[3];
  auto const out_filename = argv[4];

  auto const pymon_pic = LoadSpritesheet(pymon_pic_filename);
  auto const pymask_pic = LoadSpritesheet(pymask_pic_filename);
  auto const pymon_dat = LoadMonsterData(pymon_dat_filename);

  auto const spritesheet_width = 10;
  auto const spritesheet_height = pymon_dat.size() % spritesheet_width == 0
                                      ? (int)(pymon_dat.size() / 10)
                                      : (int)(pymon_dat.size() / 10) + 1;

  auto const atlas_width = pymon_pic[0].GetWidth() * spritesheet_width;
  auto const atlas_stride = atlas_width * kImageComponents;
  auto const atlas_height = pymon_pic[0].GetHeight() * spritesheet_height;
  auto atlas = std::vector<uint8_t>(atlas_stride * atlas_height, 0);

  for (auto i = 0; i < pymon_dat.size(); ++i) {
    auto const gfx = pymon_dat[i].gfx - 1;
    auto& image = pymon_pic[gfx];
    auto& mask = pymask_pic[gfx];

    for (auto yy = 0; yy < image.GetHeight(); ++yy) {
      for (auto xx = 0; xx < image.GetWidth(); ++xx) {
        if (mask.Get(xx, yy) != color_black) {
          continue;
        }

        auto const color = image.Get(xx, yy);

        auto const basex = (i % spritesheet_width) * image.GetWidth();
        auto const drawx = basex + xx;
        auto const basey = (i / spritesheet_width) * image.GetWidth();
        auto const drawy = basey + yy;

        auto const index = (drawy * atlas_stride) + (drawx * kImageComponents);
        auto offset = index;
        atlas[offset++] = color.r;
        atlas[offset++] = color.g;
        atlas[offset++] = color.b;
        atlas[offset++] = 255;
      }
    }
  }

  stbi_write_png(out_filename, atlas_width, atlas_height, kImageComponents,
                 atlas.data(), atlas_stride);

  return 0;
}
