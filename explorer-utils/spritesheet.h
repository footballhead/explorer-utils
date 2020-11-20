#pragma once

#include "image.h"

#include <string>

std::vector<Image> LoadCgaSpritesheet(std::string const& filename);
std::vector<Image> LoadEgaSpritesheet(std::string const& filename);
