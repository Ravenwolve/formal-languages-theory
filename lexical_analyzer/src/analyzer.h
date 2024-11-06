#pragma once

#include "lexeme.h"
#include "state.h"
#include <filesystem>
#include <vector>

class LexicalAnalyzer {
public:
  std::vector<Lexeme> Analyse(const std::string &text);

public:
  LexicalAnalyzer();

private:
  State _state;
};
