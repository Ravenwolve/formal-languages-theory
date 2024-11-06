#pragma once

#include "automaton.h"

class NondeterministicFiniteAutomaton;
class DeterministicFiniteAutomaton;

class EpsNondeterministicFiniteAutomaton;
using ENFA = EpsNondeterministicFiniteAutomaton;

class EpsNondeterministicFiniteAutomaton : public Automaton {
private:
  std::unordered_set<State> Closure(const std::unordered_set<State> &states);

public:
  static const char EPS_CHARACTER;

protected:
  EpsNondeterministicFiniteAutomaton(MovesTable &&table);

public:
  EpsNondeterministicFiniteAutomaton(
      const EpsNondeterministicFiniteAutomaton &other);
  EpsNondeterministicFiniteAutomaton(
      EpsNondeterministicFiniteAutomaton &&other);
  EpsNondeterministicFiniteAutomaton(const std::filesystem::path &file);

  operator NondeterministicFiniteAutomaton();
  operator DeterministicFiniteAutomaton();

  bool InLanguage(const std::string &word) final;

  friend class NondeterministicFiniteAutomaton;
  friend class DeterministicFiniteAutomaton;
};
