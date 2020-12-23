#include <explorer-utils/monster.h>
#include <explorer-utils/spritesheet.h>
#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 3;  // RGB
}  // namespace

int main(int argc, char** argv) {
  if (argc)

    if (argc < 4) {
      std::cerr << "Usage: " << argv[0] << " pymon.pic pymon.dat out.png\n";
      return 1;
    }

  auto const pymon_pic_filename = argv[1];
  auto const pymon_dat_filename = argv[2];
  auto const out_filename = argv[3];

  // TODO: transparency

  auto const pymon_pic = LoadSpritesheet(pymon_pic_filename);
  auto const pymon_dat = LoadMonsterData(pymon_dat_filename);

  auto const spritesheet_width = 10;
  auto const spritesheet_height = pymon_dat.size() % 10 == 0
                                      ? (int)(pymon_dat.size() / 10)
                                      : (int)(pymon_dat.size() / 10) + 1;
  auto atlas = Image{pymon_pic[0].GetWidth() * spritesheet_width,
                     pymon_pic[0].GetHeight() * spritesheet_height};

  for (auto i = 0; i < pymon_dat.size(); ++i) {
    auto const gfx = pymon_dat[i].gfx - 1;
    auto& image = pymon_pic[gfx];
    atlas.Blit(image, (i % spritesheet_width) * image.GetWidth(),
               (i / spritesheet_width) * image.GetHeight());
  }

  // XXX: I'm relying on color_t to serialize properly without
  // intervention which may not be the case on non-x86 platforms
  stbi_write_png(out_filename, atlas.GetWidth(), atlas.GetHeight(),
                 kImageComponents, atlas.GetData().data(),
                 atlas.GetWidth() * kImageComponents);

  return 0;
}
