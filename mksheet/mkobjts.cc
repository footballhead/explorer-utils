#include <explorer-utils/room.h>
#include <explorer-utils/spritesheet.h>
#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 4;  // RGBA
}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " egapics.pic out.png\n";
    return 1;
  }

  auto const egapics_filename = argv[1];
  auto const out_filename = argv[2];

  auto const egapics = LoadSpritesheet(egapics_filename);

  auto constexpr first_object = 'd';
  auto constexpr last_object = 'w';
  auto constexpr number_of_objects = last_object - first_object;

  auto constexpr spritesheet_width = 10;
  auto constexpr spritesheet_height =
      number_of_objects % spritesheet_width == 0
          ? (int)(number_of_objects / spritesheet_width)
          : (int)(number_of_objects / spritesheet_width) + 1;

  auto const atlas_width = egapics[0].GetWidth() * spritesheet_width;
  auto const atlas_stride = atlas_width * kImageComponents;
  auto const atlas_height = egapics[0].GetHeight() * spritesheet_height;
  auto atlas = std::vector<uint8_t>(atlas_stride * atlas_height, 0);

  for (auto i = 0; i < number_of_objects; ++i) {
    auto const object = first_object + i;
    auto const image_tile = GetObjectTile(object);
    auto const mask_tile = GetObjectTileMask(object);

    if (image_tile == 0) {
      continue;
    }

    auto const& image = egapics[image_tile];

    for (auto yy = 0; yy < image.GetHeight(); ++yy) {
      for (auto xx = 0; xx < image.GetWidth(); ++xx) {
        if (mask_tile && egapics[mask_tile].Get(xx, yy) != color_black) {
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
