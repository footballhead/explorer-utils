#include "file.h"

#include <fstream>
#include <iterator>
#include <stdexcept>

using namespace std::string_literals;

std::vector<uint8_t> ReadBinaryFile(std::string const& file) {
  std::ifstream in(file, std::ios_base::binary);
  if (!in.good()) {
    throw std::runtime_error("Failed to open file: "s + file);
  }

  return std::vector<uint8_t>(std::istreambuf_iterator<char>(in),
                              std::istreambuf_iterator<char>());
}
