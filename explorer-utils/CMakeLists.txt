add_library(explorer-utils STATIC
    file.cc
    image.cc
    monster.cc
    room.cc
    spritesheet.cc
    )
target_include_directories(explorer-utils PUBLIC ..)
