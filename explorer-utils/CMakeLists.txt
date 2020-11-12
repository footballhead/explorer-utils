add_library(explorer-utils STATIC
    file.cc
    image.cc
    monster.cc
    room.cc
    )
target_include_directories(explorer-utils PUBLIC ..)
