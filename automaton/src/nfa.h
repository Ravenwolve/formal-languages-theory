#pragma once

#include "automaton.h"

class EpsNondeterministicFiniteAutomaton;
class DeterministicFiniteAutomaton;

class NondeterministicFiniteAutomaton;
using NFA = NondeterministicFiniteAutomaton;

class NondeterministicFiniteAutomaton : public Automaton {
public:
  NondeterministicFiniteAutomaton(const NondeterministicFiniteAutomaton &other);
  NondeterministicFiniteAutomaton(NondeterministicFiniteAutomaton &&other);
  NondeterministicFiniteAutomaton(const std::filesystem::path &file);

  operator EpsNondeterministicFiniteAutomaton();
  operator DeterministicFiniteAutomaton();

  bool InLanguage(const std::string &word) final;

private:
  NondeterministicFiniteAutomaton(MovesTable &&table);

  friend class EpsNondeterministicFiniteAutomaton;
  friend class DeterministicFiniteAutomaton;
};
