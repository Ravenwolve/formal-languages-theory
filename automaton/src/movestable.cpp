#include "movestable.h"

void MovesTable::AddMoveToState(State state, char character,
                                States next_states) {
  if (character) {
    for (const auto &next_state : next_states) {
      table[next_state];
    }
    table[state][character].insert(next_states.begin(), next_states.end());
  } else {
    table[state];
  }
}

void MovesTable::RemoveState(State state) { table.erase(state); }

MovesTable::States MovesTable::GetNextStates(State current_state,
                                             char character) const {
  if (!(table.contains(current_state) &&
        table.at(current_state).contains(character))) {
    return {};
  }

  return table.at(current_state).at(character);
}

MovesTable::Table::iterator MovesTable::begin() { return table.begin(); }
MovesTable::Table::iterator MovesTable::end() { return table.end(); }
MovesTable::Table::const_iterator MovesTable::cbegin() const {
  return table.cbegin();
}
MovesTable::Table::const_iterator MovesTable::cend() const {
  return table.cbegin();
}

size_t MovesTable::Size() const noexcept { return table.size(); }

bool MovesTable::ContainsState(State state) const noexcept {
  return table.contains(state);
}

std::unordered_map<char, std::unordered_set<State>> &
MovesTable::operator[](State state) {
  return table[state];
}
