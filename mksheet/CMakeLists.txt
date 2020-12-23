add_executable(mksheet mksheet.cc)
target_link_libraries(mksheet explorer-utils stb_image_write)

add_executable(mkmonts mkmonts.cc)
target_link_libraries(mkmonts explorer-utils stb_image_write)