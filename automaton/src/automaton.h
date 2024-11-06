#pragma once

#include "movestable.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Automaton {
protected:
  MovesTable moves_table;

  mutable std::unordered_set<State> current_states;

public:
  Automaton(const std::filesystem::path &file);
  Automaton(const Automaton &other) = default;
  Automaton(Automaton &&other) = default;
  MovesTable GetMovesTable() const;
  virtual bool InLanguage(const std::string &word) = 0;

protected:
  Automaton(MovesTable &&table);
  bool Next(char character) const;
  bool IsFinal() const;
  friend void Out(const MovesTable &table);
};
