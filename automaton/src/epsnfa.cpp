#include "epsnfa.h"
#include "dfa.h"
#include "functional.h"
#include "log.h"
#include "nfa.h"

#include <iostream>

using States = std::unordered_set<State>;

const char ENFA::EPS_CHARACTER = '~';

ENFA::EpsNondeterministicFiniteAutomaton(MovesTable &&table)
    : Automaton(std::move(table)) {}

ENFA::EpsNondeterministicFiniteAutomaton(const ENFA &other)
    : Automaton(other) {}

ENFA::EpsNondeterministicFiniteAutomaton(ENFA &&other) : Automaton(other) {}

ENFA::EpsNondeterministicFiniteAutomaton(const std::filesystem::path &file)
    : Automaton(file) {}

States ENFA::Closure(const States &states) {
  States eps_closure = states;

  for (State state : states) {
    States next_eps_moves = Closure(moves_table[state][ENFA::EPS_CHARACTER]);
    eps_closure.insert(next_eps_moves.begin(), next_eps_moves.end());
  }

  return eps_closure;
}

bool ENFA::InLanguage(const std::string &word) {
  current_states = Closure(current_states);
  Log(current_states);

  for (auto ch : word) {
    Next(ch);
    current_states = Closure(current_states);

    Log(current_states);
    std::cerr << '\n' << "Input: " << ch << '\n';
  }

  return IsFinal();
}

ENFA::operator DeterministicFiniteAutomaton() {
  return NondeterministicFiniteAutomaton(*this);
}

static bool IsReachable(const State &state, MovesTable &table) {
  for (const auto &[source_state, moves] : table) {
    for (const auto &[character, next_state] : moves) {
      if (next_state.contains(state) && !(source_state == state)) {
        return true;
      }
    }
  }

  return false;
}

static void Minimize(MovesTable &table) {
  States unreachable;

  for (const auto &[state, moves] : table) {
    if (!(IsReachable(state, table) || state.is_initial)) {
      unreachable.insert(state);
    }
  }

  for (const State &state : unreachable) {
    table.RemoveState(state);
  }
}

ENFA::operator NondeterministicFiniteAutomaton() {
  using States = std::unordered_set<State>;

  MovesTable nfa_moves_table;

  for (const auto &[state, moves] : moves_table) {
    States closure = Closure({state});
    State source_state = state;

    for (const State &closure_state : closure) {
      if (closure_state.is_final) {
        source_state.is_final = true;
        break;
      }
    }

    nfa_moves_table.AddMoveToState(source_state, '\0', {});
  }

  for (const auto &[state, moves] : moves_table) {
    States closure = Closure({state});

    for (const State &closure_state : closure) {
      for (const auto &[character, next_states] : moves_table[closure_state]) {
        if (character != EPS_CHARACTER) {
          nfa_moves_table.AddMoveToState(state, character, next_states);
        }
      }
    }
  }

  Minimize(nfa_moves_table);

  return nfa_moves_table;
}
