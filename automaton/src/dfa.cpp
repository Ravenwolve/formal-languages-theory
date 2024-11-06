#include "dfa.h"
#include "epsnfa.h"
#include "log.h"
#include "nfa.h"

#include <iostream>

DFA::DeterministicFiniteAutomaton(MovesTable &&table)
    : Automaton(std::move(table)) {}

DFA::DeterministicFiniteAutomaton(const DFA &other) : Automaton(other) {}

DFA::DeterministicFiniteAutomaton(DFA &&other) : Automaton(other) {}

DFA::DeterministicFiniteAutomaton(const std::string &file_path)
    : Automaton(file_path) {
  if (!IsValid()) {
    moves_table = std::move(MovesTable());
    throw "Exception: Invalid DFA";
  }
}

bool DFA::IsValid() {
  for (auto [state, dict] : moves_table) {
    for (auto [character, states] : dict) {
      if (states.size() > 1) {
        return false;
      }
    }
  }

  return true;
}

bool DFA::InLanguage(const std::string &word) {
  for (auto ch : word) {
    Log(current_states);
    std::cerr << "Input: " << ch << '\n';
    Next(ch);
  }

  Log(current_states);

  return IsFinal();
}

DFA::operator EpsNondeterministicFiniteAutomaton() {
  return ENFA(GetMovesTable());
}

DFA::operator NondeterministicFiniteAutomaton() { return NFA(GetMovesTable()); }
