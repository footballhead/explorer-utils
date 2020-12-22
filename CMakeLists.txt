cmake_minimum_required(VERSION 3.14)
project(explorer-utils C CXX)

# External lib
add_subdirectory(stb_image)
add_subdirectory(stb_image_write)

# Libs
add_subdirectory(explorer-utils)

# Bins
add_subdirectory(maskconv)
add_subdirectory(pic2png)
add_subdirectory(rms2png)
