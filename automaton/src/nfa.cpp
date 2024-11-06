#include "nfa.h"
#include "dfa.h"
#include "epsnfa.h"
#include "functional.h"
#include "log.h"
#include "out.h"

#include <iostream>
#include <queue>

NFA::NondeterministicFiniteAutomaton(MovesTable &&table)
    : Automaton(std::move(table)) {}

NFA::NondeterministicFiniteAutomaton(const NFA &other) : Automaton(other) {}

NFA::NondeterministicFiniteAutomaton(NFA &&other) : Automaton(other) {}

NFA::NondeterministicFiniteAutomaton(const std::filesystem::path &file)
    : Automaton(file) {}

bool NFA::InLanguage(const std::string &word) {
  for (auto ch : word) {
    Log(current_states);
    std::cerr << '\n' << "Input: " << ch << '\n';
    Next(ch);
  }

  Log(current_states);

  return IsFinal();
}

NFA::operator EpsNondeterministicFiniteAutomaton() {
  return ENFA(GetMovesTable());
}

static std::unordered_set<State> GetStatesList(MovesTable &table) {
  std::unordered_set<State> states;

  for (auto [state, dict] : table) {
    states.insert(state);
  }

  return states;
}

static void MergeStates(MovesTable &source_table,
                        const std::unordered_set<State> &source_states) {
  State new_state{0, false, false};
  for (auto state : source_states) {
    if (state.is_final) {
      new_state.is_final = true;
      break;
    }
  }

  while (true) {
    size_t id = std::rand() % (source_table.Size() * 10);
    if (!source_table.ContainsState(State{id, false, false})) {
      new_state.id = id;
      break;
    }
  }

  // NOTE: Add every move from old states to new state
  for (auto state : source_states) {
    for (auto [character, next_states] : source_table[state]) {
      for (auto next_state : next_states) {
        source_table[new_state][character].insert(next_state);
      }
    }
  }

  // NOTE: Replace states list to new state
  for (auto [state, dict] : source_table) {
    for (auto [character, next_states] : dict) {
      if (next_states == source_states) {
        dict[character] = {new_state};
      }
    }
  }
}

NFA::operator DeterministicFiniteAutomaton() {
  MovesTable dfa_moves_table;

  using States = std::unordered_set<State>;

  std::queue<std::tuple<State, char, States>> unprocessed_moves;

  const State current_source_state = *current_states.begin();

  std::unordered_map<States, State> mapping;
  mapping[{current_source_state}] = current_source_state;

  for (const auto &[character, destination_states] :
       moves_table[current_source_state]) {
    unprocessed_moves.push(
        {current_source_state, character, destination_states});
  }

  while (!unprocessed_moves.empty()) {
    const auto [current_source_state, character, destination_states] =
        unprocessed_moves.front();
    unprocessed_moves.pop();

    State combined_destination_state =
        CombineStates(destination_states, mapping, moves_table);

    dfa_moves_table.AddMoveToState(current_source_state, character,
                                   {combined_destination_state});

    if (current_source_state == combined_destination_state) {
      continue;
    }

    std::unordered_map<char, States>
        merged_next_moves_of_combined_destination_state =
            JoinMovesByCharacter(moves_table, destination_states);

    for (const auto &[character, next_destination_states] :
         merged_next_moves_of_combined_destination_state) {
      if (!(next_destination_states.size() == 1 &&
            combined_destination_state == *next_destination_states.begin()))
        unprocessed_moves.push(
            {combined_destination_state, character, next_destination_states});
    }

    std::cout << "\tCurrent source state\t" << current_source_state.id
              << std::endl;
    for (auto item : destination_states) {
      std::cout << item.id << " ";
    }
    std::cout << std::endl;
    std::cout << "\tCombined state\t" << combined_destination_state.id
              << std::endl;

    Out(MovesTable(dfa_moves_table));
  }

  return dfa_moves_table;
}
