# explorer-utils

Collection of tools for decoding files from Dungeon Explorer (DOS).

Each folder has a README explaining the tool.

## Requirements

* CMake
* C++17 compiler
* SDL2 (for certain tools like `rmsedit`)

## Building

```
mkdir build
cd build
cmake ..
make
```

On Windows, I recommend Visual Studio Code with the CMake Tools extension.

## Contributing

Code must be `clang-format`'d with the Google style (except external code like `stb_image`, `stb_image_write`, etc).

## License

See [UNLICENSE](UNLICENSE)
