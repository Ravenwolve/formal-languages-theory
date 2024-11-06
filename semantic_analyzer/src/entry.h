#pragma once

#include <cstdint>
#include <string>
#include <variant>

struct Entry {
  enum Command {
    JMP,
    JZ,
    MOV,
    CMPE,
    CMPNE,
    CMPL,
    CMPG,
    AND,
    OR,
    ADD,
    SUB,
    MUL,
    DIV,
    INPUT,
    OUTPUT,
    BRACKET,
  };
  enum EntryType { COMMAND, VARIABLE, CONSTANT, INSTRUCTION_POINTER } type;
  std::variant<Command, std::string, int> data;
};
