#pragma once

#include "state.h"

#include <unordered_map>
#include <unordered_set>

class MovesTable {

protected:
  template <typename Key, typename Tp> using Map = std::unordered_map<Key, Tp>;
  using States = std::unordered_set<State>;
  using Table = Map<State, Map<char, States>>;

  Table table;

public:
  MovesTable() = default;
  MovesTable(const MovesTable &other) = default;
  MovesTable(MovesTable &&other) = default;

  MovesTable &operator=(const MovesTable &other) = default;
  MovesTable &operator=(MovesTable &&other) = default;

  States GetNextStates(State current_state, char character) const;

  void AddMoveToState(State state, char character, States next_states);
  void RemoveState(State state);

  Table::iterator begin();
  Table::iterator end();
  Table::const_iterator cbegin() const;
  Table::const_iterator cend() const;

  size_t Size() const noexcept;

  bool ContainsState(State state) const noexcept;

  Map<char, States> &operator[](State state);
};
