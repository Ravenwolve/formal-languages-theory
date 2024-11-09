#pragma once

#include "entry.h"
#include <stack>
#include <string>
#include <vector>

class PolishInverseRecord {
public:
  size_t Push(Entry::Command command);
  size_t ForcePush(Entry::Command command);
  size_t PushVariable(const std::string &value);
  size_t PushConstant(const std::string &value);
  size_t PushAddress(int value);
  void RewriteAddress(size_t index, int value);
  size_t PushOpeningParenthesis();
  size_t PushClosingParenthesis();
  size_t Final();
  std::vector<Entry> ToVector();
  void Clear();
  size_t Size();

private:
  std::vector<Entry> entries;
  std::stack<Entry::Command> stack;
};
