#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint8_t> read_binary_file(std::string const& file);

void dump_binary_file(std::string const& file, std::vector<uint8_t> const& data);
