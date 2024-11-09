#include "entrycollection.h"
#include <iostream>
#include <unordered_map>

static const std::unordered_map<Entry::Command, size_t> PRIORITY{
    {Entry::Command::OR, 1},     {Entry::Command::AND, 2},
    {Entry::Command::MOV, 0},    {Entry::Command::CMPE, 3},
    {Entry::Command::CMPNE, 3},  {Entry::Command::CMPG, 3},
    {Entry::Command::CMPL, 3},   {Entry::Command::INPUT, 1},
    {Entry::Command::OUTPUT, 1}, {Entry::Command::INPUT, 1},
    {Entry::Command::ADD, 1},    {Entry::Command::SUB, 1},
    {Entry::Command::MUL, 2},    {Entry::Command::DIV, 2},
    {Entry::Command::JZ, 0},     {Entry::Command::JMP, 0}};

size_t PolishInverseRecord::Push(Entry::Command command) {
  while (!stack.empty() && stack.top() != Entry::Command::BRACKET &&
         PRIORITY.at(stack.top()) >= PRIORITY.at(command)) {
    entries.push_back({Entry::EntryType::COMMAND, stack.top()});
    stack.pop();
  }

  stack.push(command);

  return entries.size() - 1;
}

size_t PolishInverseRecord::PushVariable(const std::string &value) {
  entries.push_back({Entry::EntryType::VARIABLE, value});

  return entries.size() - 1;
}

size_t PolishInverseRecord::PushConstant(const std::string &value) {
  int constant;

  try {
    constant = std::stoi(value);
  } catch (const std::out_of_range &ex) {
    std::cerr << "semantic error: constant " << value << " out of range";
    exit(1);
  }

  entries.push_back({Entry::EntryType::CONSTANT, constant});

  return entries.size() - 1;
}

size_t PolishInverseRecord::PushAddress(int value) {
  entries.push_back({Entry::EntryType::INSTRUCTION_POINTER, value});

  return entries.size() - 1;
}

size_t PolishInverseRecord::PushOpeningParenthesis() {
  stack.push(Entry::Command::BRACKET);

  return entries.size() - 1;
}
size_t PolishInverseRecord::PushClosingParenthesis() {
  while (!stack.empty() && stack.top() != Entry::Command::BRACKET) {
    entries.push_back({Entry::EntryType::COMMAND, stack.top()});
    stack.pop();
  }
  stack.pop();

  return entries.size() - 1;
}
size_t PolishInverseRecord::Final() {
  while (!stack.empty()) {
    entries.push_back({Entry::EntryType::COMMAND, stack.top()});
    stack.pop();
  }

  return entries.size() - 1;
}

size_t PolishInverseRecord::ForcePush(Entry::Command command) {
  entries.push_back({Entry::EntryType::COMMAND, command});

  return entries.size() - 1;
}

std::vector<Entry> PolishInverseRecord::ToVector() { return entries; }

void PolishInverseRecord::Clear() {
  entries.clear();

  while (!stack.empty()) {
    stack.pop();
  }
}

size_t PolishInverseRecord::Size() { return entries.size(); }

void PolishInverseRecord::RewriteAddress(size_t index, int value) {
  if (index >= entries.size()) {
    return;
  }

  entries[index] = {Entry::EntryType::INSTRUCTION_POINTER, value};
}
