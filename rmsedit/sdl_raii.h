#pragma once

#include <SDL.h>

#include <memory>

//
// SDL_raii
//

struct SDL_raii final {
  SDL_raii() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      throw std::runtime_error{SDL_GetError()};
    }
  }

  ~SDL_raii() { SDL_Quit(); }
};

//
// SDL_Window_ptr
//

struct SDL_Window_deleter {
  void operator()(SDL_Window* other) const { SDL_DestroyWindow(other); }
};

using SDL_Window_ptr = std::unique_ptr<SDL_Window, SDL_Window_deleter>;

inline auto make_SDL_Window(SDL_Window* to_wrap) {
  return SDL_Window_ptr{to_wrap, SDL_Window_deleter{}};
}

//
// SDL_Renderer_ptr
//

struct SDL_Renderer_deleter {
  void operator()(SDL_Renderer* other) const { SDL_DestroyRenderer(other); }
};

using SDL_Renderer_ptr = std::unique_ptr<SDL_Renderer, SDL_Renderer_deleter>;

inline auto make_SDL_Renderer(SDL_Renderer* to_wrap) {
  return SDL_Renderer_ptr{to_wrap, SDL_Renderer_deleter{}};
}

//
// SDL_Surface_ptr
//

struct SDL_Surface_deleter {
  void operator()(SDL_Surface* other) const { SDL_FreeSurface(other); }
};

using SDL_Surface_ptr = std::unique_ptr<SDL_Surface, SDL_Surface_deleter>;

inline auto make_SDL_Surface(SDL_Surface* to_wrap) {
  return SDL_Surface_ptr{to_wrap, SDL_Surface_deleter{}};
}

//
// SDL_Texture_ptr
//

struct SDL_Texture_deleter {
  void operator()(SDL_Texture* other) const { SDL_DestroyTexture(other); }
};

using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, SDL_Texture_deleter>;

inline auto make_SDL_Texture(SDL_Texture* to_wrap) {
  return SDL_Texture_ptr{to_wrap, SDL_Texture_deleter{}};
}
