#pragma once

#include <vector>

enum Resource { IRON = 0, GOLD = 1, GEMS = 2, EXP = 3 };

extern const char* const RES_NAMES[4];
extern const int RES_VALUES[4];

struct Room {
  std::vector<int> adjacent_rooms;  // sorted ascending, unique
  int resources[4] = {0, 0, 0, 0};  // iron, gold, gems, exp
};

struct Dungeon {
  int N = 0;                // highest room id; total rooms = N + 1
  std::vector<Room> rooms;  // index = room id
  int food = 0;             // M
  int target = -1;          // resource index whose value is doubled
};

int resourceValue(int idx, int target);