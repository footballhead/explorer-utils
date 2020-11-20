#include <explorer-utils/spritesheet.h>

#include <stb_image_write/stb_image_write.h>

#include <iostream>
#include <string>

namespace {
constexpr auto kImageComponents = 3; // RGB
} // namespace

int main(int argc, char** argv)
{
    if (argc )

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " in.pic out_prefix\n";
        return 1;
    }

    auto const in_filename = argv[1];
    auto const out_prefix = argv[2];

    auto images = LoadSpritesheet(in_filename);

    for (auto i = 0; i < images.size(); ++i) {
        auto const out_filename = out_prefix + std::to_string(i) + ".png";
        auto const& image = images[i];
        // XXX: I'm relying on color_t to serialize properly without
        // intervention which may not be the case on non-x86 platforms
        stbi_write_png(out_filename.c_str(), image.GetWidth(), image.GetHeight(), kImageComponents, image.GetData().data(), image.GetWidth() * kImageComponents);
    }

    return 0;
}
