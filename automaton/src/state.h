#pragma once

#include <cstdint>

#include <functional>

struct State {
  std::size_t id;
  bool is_initial;
  bool is_final;

  bool operator==(const State &other) const;
  State &operator=(const State &other);
};

template <> struct std::hash<State> {
  std::size_t operator()(const State &key) const {
    return std::hash<size_t>()(key.id);
  }
};
