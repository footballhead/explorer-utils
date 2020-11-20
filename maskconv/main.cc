#include <explorer-utils/file.h>
#include <explorer-utils/spritesheet.h>
#include <stb_image_write/stb_image_write.h>

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " pymask.pic cgamask.pic\n";
    std::cerr << "Converts pymask.pic from EGA to CGA and stores it in to "
                 "cgamask.pic\n";
    return 1;
  }

  auto const pymask_filename = argv[1];
  auto const cgamask_filename = argv[2];

  auto const pymask_images = LoadEgaSpritesheet(pymask_filename);

  std::ofstream out(cgamask_filename, std::ios_base::binary);
  if (!out.good()) {
    std::cerr << "Couldn't open: " << cgamask_filename << "\n";
    return 1;
  }

  for (auto i = 0; i < pymask_images.size(); ++i) {
    auto const& img = pymask_images[i];

    // First, write "CGA header". If this is missing then the data will be
    // ignored.
    out.put(0x0E);
    out.put(0x00);
    out.put(0x0E);
    out.put(0x00);

    // Then Dump image (fit width=15 into cell of size 16)
    for (auto y = 0; y < img.GetHeight(); ++y) {
      for (auto x = 0; x < img.GetWidth(); x += 4) {
        // TODO more generic function that handles incomplete rows
        auto c = (img.Get(x, y) == color_white ? 0b11000000 : 0) |
                 (img.Get(x + 1, y) == color_white ? 0b00110000 : 0) |
                 (img.Get(x + 2, y) == color_white ? 0b00001100 : 0);

        // Non-full row (can happen because image data is 15x15)
        if (img.GetWidth() - x != 3) {
          c |= img.Get(x + 3, y) == color_white ? 0b00000011 : 0;
        }

        out.put(c);
      }
    }

    // finally Write 192 bytes of garbage
    char padding[192];
    out.write(padding, sizeof(padding));
  }

  return 0;
}
