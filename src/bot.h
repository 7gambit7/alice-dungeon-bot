#pragma once

#include <iosfwd>

#include "domain.h"

class IBot {
 public:
  virtual ~IBot() = default;
  // Takes Dungeon by value: bot mutates resource counts during simulation
  virtual void run(Dungeon dungeon, std::ostream& out) = 0;
};