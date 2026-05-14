#pragma once

#include <vector>

#include "bot.h"

class AliceBot : public IBot {
 public:
  void run(Dungeon dungeon, std::ostream& out) override;

 private:
  int chooseNextExplore(int current, const std::vector<bool>& visited,
                        const std::vector<Room>& rooms);
};