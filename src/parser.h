#pragma once

#include <string>

#include "domain.h"

struct ParseResult {
  bool ok = false;
  std::string badLine;
};

ParseResult parseInput(const std::string& filename, Dungeon& d);