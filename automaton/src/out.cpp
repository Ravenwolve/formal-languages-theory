#include "out.h"

#include <iostream>

static const char EPS_CHARACTER = '~';

static size_t CountDigits(size_t number) {
  size_t counter = 1;

  while ((number /= 10) != 0) {
    ++counter;
  }

  return counter;
}

static void OutHeader(size_t size_of_state,
                      std::unordered_map<char, size_t> sizes_of_sections) {
  for (size_t i = 0; i < size_of_state; ++i) {
    std::cout << ' ';
  }

  for (const auto &[character, size] : sizes_of_sections) {
    if (character != EPS_CHARACTER) {
      for (int i = 0; i < size; ++i) {
        std::cout << ' ';
      }
      std::cout << character;
    }
  }

  if (sizes_of_sections.contains(EPS_CHARACTER)) {
    for (int i = 0; i < sizes_of_sections.at(EPS_CHARACTER); ++i) {
      std::cout << ' ';
    }
    std::cout << "eps";
  }

  std::cout << '\n';
}

static void
OutTableFrameRow(size_t size_of_state,
                 const std::unordered_map<char, size_t> &sizes_of_sections,
                 const std::string &opening, const std::string &node,
                 const std::string &closing) {
  for (size_t i = 0; i < size_of_state; ++i) {
    std::cout << ' ';
  }

  std::cout << opening;

  for (const auto &[character, size] : sizes_of_sections) {
    if (character != EPS_CHARACTER) {
      for (size_t j = 0; j < size; ++j) {
        std::cout << "─";
      }
      std::cout << node;
    }
  }

  if (sizes_of_sections.contains(EPS_CHARACTER)) {
    for (size_t j = 0; j < sizes_of_sections.at(EPS_CHARACTER); ++j) {
      std::cout << "─";
    }
    std::cout << node;
  }

  std::cout << closing << '\n';
}

void Out(MovesTable &&table) {
  // NOTE: max states counting for wide of output table
  std::unordered_map<char, size_t> columns_widths_dict;
  size_t max_size_of_state = 0;

  for (const auto &[source_state, dict] : table) {
    size_t size_of_current_state = CountDigits(source_state.id);
    if (size_of_current_state > max_size_of_state) {
      max_size_of_state = size_of_current_state;
    }

    for (const auto &[character, destination_states] : dict) {
      if (destination_states.size() > columns_widths_dict[character]) {
        columns_widths_dict[character] = destination_states.size();
      }
    }
  }

  for (auto &[character, size] : columns_widths_dict) {
    size *= (max_size_of_state + 1);
  }

  OutHeader((max_size_of_state + 1) / 2, columns_widths_dict);

  OutTableFrameRow(max_size_of_state + 1, columns_widths_dict, "┌", "┬", "┐");

  for (auto [source_state, dict] : table) {
    size_t remained = max_size_of_state + 1;

    if (source_state.is_initial) {
      std::cout << '>';
      --remained;
    }

    if (source_state.is_final) {
      std::cout << '*';
      --remained;
    }

    std::cout << source_state.id;

    remained -= CountDigits(source_state.id);

    for (size_t i = 0; i < remained; ++i) {
      std::cout << ' ';
    }

    std::cout << "│";

    for (const auto &[character, size] : columns_widths_dict) {
      if (character != EPS_CHARACTER) {
        remained = size;
        for (auto state : dict[character]) {
          std::cout << state.id;

          remained -= CountDigits(state.id);

          std::cout << ' ';
          --remained;
        }

        for (size_t i = 0; i < remained; ++i) {
          std::cout << ' ';
        }

        std::cout << "│";
      }
    }

    if (columns_widths_dict.contains(EPS_CHARACTER)) {
      remained = columns_widths_dict[EPS_CHARACTER];
      for (auto state : dict[EPS_CHARACTER]) {
        std::cout << state.id;

        remained -= CountDigits(state.id);

        std::cout << ' ';
        --remained;
      }

      for (size_t i = 0; i < remained; ++i) {
        std::cout << ' ';
      }

      std::cout << "│";
    }

    std::cout << "│\n";

    OutTableFrameRow(max_size_of_state + 1, columns_widths_dict, "├", "┼", "┤");
  }

  OutTableFrameRow(max_size_of_state + 1, columns_widths_dict, "└", "┴", "┘");
}
