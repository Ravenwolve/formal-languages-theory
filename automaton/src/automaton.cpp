#include "automaton.h"

#include <fstream>
#include <nlohmann/json.hpp>

Automaton::Automaton(MovesTable &&table) : moves_table(std::move(table)) {
  for (const auto &[state, dict] : moves_table) {
    if (state.is_initial) {
      current_states.insert(state);
    }
  }
}

Automaton::Automaton(const std::filesystem::path &file) {
  std::ifstream in(file);

  if (!in.is_open()) {
    return;
  }

  std::unordered_map<size_t, State> states;

  auto json_moves = nlohmann::json::parse(in);

  in.close();

  for (auto json_state : json_moves) {
    State source_state{json_state.at("source_state"),
                       json_state.at("is_initial_state"),
                       json_state.at("is_final_state")};
    states[source_state.id] = source_state;

    if (source_state.is_initial) {
      current_states.insert(source_state);
    }
  }

  for (auto json_state : json_moves) {
    size_t id = json_state.at("source_state");

    moves_table.AddMoveToState(states[id], '\0', {});

    for (auto json_move : json_state.at("moves")) {
      std::string character = json_move.at("character");

      std::unordered_set<State> next_states;

      for (auto json_next_state_id : json_move.at("next_states")) {
        next_states.insert(states[static_cast<size_t>(json_next_state_id)]);
      }

      moves_table.AddMoveToState(states[id], character[0], next_states);
    }
  }
}

bool Automaton::Next(char character) const {
  std::unordered_set<State> next_states;

  for (auto &current_state : current_states) {
    auto next_states_of_current_state =
        moves_table.GetNextStates(current_state, character);

    for (auto &state : next_states_of_current_state) {
      next_states.insert(state);
    }
  }

  bool states_changed = next_states == current_states;

  current_states = std::move(next_states);

  return states_changed;
}

bool Automaton::IsFinal() const {
  for (auto state : current_states) {
    if (state.is_final) {
      return true;
    }
  }

  return false;
}

MovesTable Automaton::GetMovesTable() const { return moves_table; }
