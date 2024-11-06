#include "movestable.h"
#include "state.h"
#include <unordered_set>

template <> struct std::hash<std::unordered_set<State>> {
  std::size_t operator()(const std::unordered_set<State> &value) const {
    const std::size_t prime = 19937;
    const std::hash<State> hash;
    std::size_t result = 0;
    for (auto &&v : value) {
      result ^= prime * hash(v);
    }
    return result;
  }
};

std::unordered_map<char, std::unordered_set<State>>
JoinMovesByCharacter(MovesTable &table,
                     const std::unordered_set<State> &states);

State CombineStates(
    const std::unordered_set<State> &states,
    std::unordered_map<std::unordered_set<State>, State> &mapping,
    MovesTable &table);
