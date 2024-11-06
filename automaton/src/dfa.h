#pragma once

#include "automaton.h"

class EpsNondeterministicFiniteAutomaton;
class NondeterministicFiniteAutomaton;

class DeterministicFiniteAutomaton;
using DFA = DeterministicFiniteAutomaton;

class DeterministicFiniteAutomaton : public Automaton {
public:
  DeterministicFiniteAutomaton(const DeterministicFiniteAutomaton &other);
  DeterministicFiniteAutomaton(DeterministicFiniteAutomaton &&other);
  DeterministicFiniteAutomaton(const std::string &file_path);

  operator EpsNondeterministicFiniteAutomaton();
  operator NondeterministicFiniteAutomaton();

  bool InLanguage(const std::string &word) final;

private:
  DeterministicFiniteAutomaton(MovesTable &&table);

  bool IsValid();

  friend class EpsNondeterministicFiniteAutomaton;
  friend class NondeterministicFiniteAutomaton;
};
