add_library(explorer-utils STATIC
    file.cc
    image.cc
    room.cc
    )
target_include_directories(explorer-utils PUBLIC ..)
