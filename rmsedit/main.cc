#include <explorer-utils/file.h>
#include <explorer-utils/image.h>
#include <explorer-utils/monster.h>
#include <explorer-utils/room.h>
#include <explorer-utils/spritesheet.h>

#include <iostream>
#include <string>

#include "sdl_raii.h"

namespace {

constexpr auto kImageWidth = 15;
constexpr auto kImageHeight = 15;

SDL_Surface_ptr ToSdlSurface(Image const& image,
                             Image const& mask_image = Image(kImageWidth,
                                                             kImageHeight)) {
  // The 32 bit packing makes the following for loop line up the data properly
  auto surface =
      SDL_CreateRGBSurface(0, image.GetWidth(), image.GetHeight(), 32,
                           0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  if (!surface) {
    std::cerr << "SDL_CreateRGBSurface failed\n";
    return nullptr;
  }

  for (auto y = 0; y < image.GetHeight(); ++y) {
    for (auto x = 0; x < image.GetWidth(); ++x) {
      auto const color = image.Get(x, y);
      auto const mask_color = mask_image.Get(x, y);
      Uint8 const alpha = mask_color == color_black ? 255 : 0;

      auto pixels = reinterpret_cast<Uint32*>(surface->pixels);
      pixels[y * image.GetWidth() + x] =
          SDL_MapRGBA(surface->format, color.r, color.g, color.b, alpha);
    }
  }

  return make_SDL_Surface(surface);
}

SDL_Texture_ptr ToSdlTexture(SDL_Renderer* renderer, Image const& image,
                             Image const& mask_image = Image(kImageWidth,
                                                             kImageHeight)) {
  auto surface = ToSdlSurface(image, mask_image);
  if (!surface) {
    std::cerr << "toSdlSurface failed\n";
    return nullptr;
  }

  auto texture = SDL_CreateTextureFromSurface(renderer, surface.get());
  if (!texture) {
    std::cerr << "SDL_CreateTextureFromSurface failed\n";
    return nullptr;
  }

  return make_SDL_Texture(texture);
}

std::vector<SDL_Texture_ptr> ConvertSpritesheet(
    SDL_Renderer* renderer, std::vector<Image> const& images,
    std::vector<Image> const& mask_images = {}) {
  std::vector<SDL_Texture_ptr> builder;

  for (auto i = 0; i < images.size(); ++i) {
    auto texture = i < mask_images.size()
                       ? ToSdlTexture(renderer, images[i], mask_images[i])
                       : ToSdlTexture(renderer, images[i]);
    if (!texture) {
      std::cerr << "ToSdlTexture failed\n";
      return {};
    }
    builder.emplace_back(std::move(texture));
  }

  return builder;
}

std::vector<SDL_Texture_ptr> LoadAndConvertSpritesheet(
    SDL_Renderer* renderer, std::string const& filename,
    std::string const& mask_filename) {
  auto const images = LoadSpritesheet(filename);
  auto const mask_images = LoadSpritesheet(mask_filename);
  return ConvertSpritesheet(renderer, images, mask_images);
}

void DrawRectangle(SDL_Renderer* renderer, SDL_Color const& color,
                   SDL_Rect const& rect) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
}

void DrawStyledBox(SDL_Renderer* renderer, SDL_Rect const& rect) {
  SDL_Color constexpr border_color = {0x55, 0x55, 0xFF, 0xFF};
  SDL_Color constexpr black_color = {0, 0, 0, 0xFF};
  DrawRectangle(renderer, border_color, rect);
  DrawRectangle(renderer, black_color,
                SDL_Rect{rect.x + 1, rect.y + 2, rect.w - 2, rect.h - 4});
  DrawRectangle(renderer, border_color,
                SDL_Rect{rect.x + 2, rect.y + 4, rect.w - 4, rect.h - 8});
  DrawRectangle(renderer, black_color,
                SDL_Rect{rect.x + 3, rect.y + 6, rect.w - 6, rect.h - 12});
}

int GetTileMask(uint8_t tile) {
  switch (tile) {
    case 10:  // Attack effect
      return 64;
    case 11:  // Hit explosion
      return 69;
    case 18:  // Old bones
      return 71;
    case 22:  // Treasure chest
      return 65;
    case 23:  // Old body
      return 70;
    case 24:  // Player
      return 68;
    case 47:  // Smoke
      return 67;
    case 50:  // Old stone coffin
      return 72;
    case 55:  // Old grave
      return 66;
    case 60:  // ???
      return 82;
    case 75:  // ???
      return 73;
    case 76:  // ???
      return 74;
    case 83:  // Some old blood
      return 84;
  }

  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  SDL_raii sdl;

  auto window =
      make_SDL_Window(SDL_CreateWindow("RMS Editor", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, 640, 400, 0));
  if (!window) {
    std::cerr << "SDL_CreateWindow failed\n";
    return 1;
  }

  auto renderer = make_SDL_Renderer(SDL_CreateRenderer(window.get(), -1, 0));
  if (!renderer) {
    std::cerr << "SDL_CreateRenderer failed\n";
    return 1;
  }

  // This whole special song-and-dance is necessary because the mask is stored
  // randomly in the sprite sheet
  auto tile_images = std::vector<SDL_Texture_ptr>{};
  {
    auto const images = LoadSpritesheet("EGAPICS.PIC");
    for (auto tile_index = 1; tile_index <= images.size(); ++tile_index) {
      auto mask_index = GetTileMask(tile_index);

      SDL_Surface_ptr surface = nullptr;
      if (mask_index > 0) {
        surface = ToSdlSurface(images[tile_index - 1], images[mask_index - 1]);
      } else {
        surface = ToSdlSurface(images[tile_index - 1]);
      }

      auto texture =
          SDL_CreateTextureFromSurface(renderer.get(), surface.get());
      if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed\n";
        return 1;
      }
      tile_images.emplace_back(make_SDL_Texture(texture));
    }
  }

  auto const monster_images =
      LoadAndConvertSpritesheet(renderer.get(), "PYMON.PIC", "PYMASK.PIC");
  if (monster_images.empty()) {
    std::cerr << "LoadAndConvertEgaSpritesheet failed\n";
    return 1;
  }

  auto const rooms = LoadRooms("DUNGEON.RMS");
  auto const monsters = LoadMonsterData("PYMON.DAT");

  auto room_index = 0;
  auto running = true;

  // Key handling function, split out of main loop to reduce indentation
  auto do_key = [&room_index, &rooms](SDL_Keycode const keycode) {
    auto const& room = rooms[room_index];

    switch (keycode) {
      case SDLK_UP:
        if (room.nav.north != 0) {
          room_index = room.nav.north - 1;
        }
        break;
      case SDLK_DOWN:
        if (room.nav.south != 0) {
          room_index = room.nav.south - 1;
        }
        break;
      case SDLK_LEFT:
        if (room.nav.west != 0) {
          room_index = room.nav.west - 1;
        }
        break;
      case SDLK_RIGHT:
        if (room.nav.east != 0) {
          room_index = room.nav.east - 1;
        }
        break;
      case SDLK_c:
        if (room.nav.up != 0) {
          room_index = room.nav.up - 1;
        } else if (room.nav.down != 0) {
          room_index = room.nav.down - 1;
        }
        break;
    }
  };

  SDL_Event event;
  while (running && SDL_WaitEvent(&event)) {
    auto const& room = rooms[room_index];

    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        do_key(event.key.keysym.sym);
        break;
    }

    // Only draw on new events
    constexpr auto kImageDrawWidth = kImageWidth * 2;
    constexpr auto kImageDrawHeight = kImageHeight * 2;

    constexpr auto kRoomDrawX = 20;
    constexpr auto kRoomDrawY = 6;

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer.get());

    DrawStyledBox(renderer.get(), SDL_Rect{14, 0, 613, 252});

    for (auto y = 0; y < kRoomHeight; ++y) {
      for (auto x = 0; x < kRoomWidth; ++x) {
        auto const tile = room.GetTile(x, y) - 1;
        auto dest = SDL_Rect{kRoomDrawX + x * kImageDrawWidth,
                             kRoomDrawY + y * kImageDrawHeight, kImageDrawWidth,
                             kImageDrawHeight};
        SDL_RenderCopy(renderer.get(), tile_images[tile].get(), nullptr, &dest);

        auto const object_type = room.GetObjectType(x, y);
        switch (object_type) {
          case room_t::object_type::none:
            // do nothing
            break;
          case room_t::object_type::monster:
            SDL_RenderCopy(
                renderer.get(),
                monster_images[monsters[room.monster_id - 1].gfx - 1].get(),
                nullptr, &dest);
            break;
          case room_t::object_type::object:
            SDL_RenderCopy(renderer.get(),
                           tile_images[room.GetObject(x, y)].get(), nullptr,
                           &dest);
            break;
        }
      }
    }

    DrawStyledBox(renderer.get(), SDL_Rect{0, 250, 639, 144});

    SDL_RenderPresent(renderer.get());
  }

  return 0;
}
