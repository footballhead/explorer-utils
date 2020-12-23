#include <explorer-utils/spritesheet.h>
#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 3;  // RGB
}  // namespace

int main(int argc, char** argv) {
  if (argc)

    if (argc < 3) {
      std::cerr << "Usage: " << argv[0] << " in.pic out.png\n";
      return 1;
    }

  auto const in_filename = argv[1];
  auto const out_filename = argv[2];

  auto images = LoadSpritesheet(in_filename);

  auto const spritesheet_width = 10;
  auto const spritesheet_height = images.size() % 10 == 0
                                      ? (int)(images.size() / 10)
                                      : (int)(images.size() / 10) + 1;
  auto atlas = Image{images[0].GetWidth() * spritesheet_width,
                     images[0].GetHeight() * spritesheet_height};

  for (auto i = 0; i < images.size(); ++i) {
    auto& image = images[i];
    atlas.Blit(images[i], (i % spritesheet_width) * image.GetWidth(),
               (i / spritesheet_width) * image.GetHeight());
  }

  // XXX: I'm relying on color_t to serialize properly without
  // intervention which may not be the case on non-x86 platforms
  stbi_write_png(out_filename, atlas.GetWidth(), atlas.GetHeight(),
                 kImageComponents, atlas.GetData().data(),
                 atlas.GetWidth() * kImageComponents);

  return 0;
}
