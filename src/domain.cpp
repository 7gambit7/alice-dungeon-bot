#include "domain.h"

const char* const RES_NAMES[4] = {"iron", "gold", "gems", "exp"};
const int RES_VALUES[4] = {7, 11, 23, 1};

int resourceValue(int idx, int target) {
  int v = RES_VALUES[idx];
  if (idx == target) v *= 2;
  return v;
}