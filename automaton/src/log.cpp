#include "log.h"

#include <iostream>

void Log(const std::unordered_set<State> &states) {
  std::cerr << "States: ";
  for (auto state : states) {
    std::cerr << ' ' << state.id;
  }
  std::cerr << std::endl;
}
