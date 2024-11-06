#include "state.h"

bool State::operator==(const State &other) const { return id == other.id; }

State &State::operator=(const State &other) {
  id = other.id;
  is_initial = other.is_initial;
  is_final = other.is_final;
  return *this;
}
