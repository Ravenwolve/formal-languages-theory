#include "functional.h"

#include <cstdlib>

std::unordered_map<char, std::unordered_set<State>>
JoinMovesByCharacter(MovesTable &table,
                     const std::unordered_set<State> &states) {

  std::unordered_map<char, std::unordered_set<State>> merged_moves_by_character;

  for (const auto &state : states) {
    for (const auto &[character, next_states] : table[state]) {
      merged_moves_by_character[character].insert(next_states.cbegin(),
                                                  next_states.cend());
    }
  }

  return merged_moves_by_character;
}

State CombineStates(
    const std::unordered_set<State> &states,
    std::unordered_map<std::unordered_set<State>, State> &mapping,
    MovesTable &table) {
  if (mapping.contains(states)) {
    return mapping[states];
  } else if (states.size() == 1) {
    mapping[states] = *states.begin();

    return *states.begin();
  } else {
    State new_state{mapping.size(), false, false};
    do {
      new_state.id = static_cast<size_t>(std::rand()) % (table.Size() * 10);
    } while (table.ContainsState(new_state));

    for (const auto &state : states) {
      if (state.is_final) {
        new_state.is_final = true;
      }
    }

    mapping[states] = new_state;

    return new_state;
  }
}
