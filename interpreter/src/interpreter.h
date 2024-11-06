#pragma once

#include "../../semantic_analyzer/src/parser.h"
#include <optional>

class Interpreter {
public:
  using CallbackOperator = int(int, int);
  bool Interprete(const std::string &text);

private:
  int Value(const std::variant<int, std::string> &operand);
  void Debug(Entry entry);

public:
  SyntacticParser parser;
  std::stack<std::variant<int, std::string>> stack;
  std::unordered_map<std::string, int> variables;
};
