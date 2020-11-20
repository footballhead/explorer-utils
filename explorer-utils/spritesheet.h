#pragma once

#include <string>

#include "image.h"

std::vector<Image> LoadCgaSpritesheet(std::string const& filename);
std::vector<Image> LoadEgaSpritesheet(std::string const& filename);

/// Autodetects the file format and loads with the appropriate palette.
std::vector<Image> LoadSpritesheet(std::string const& filename);
