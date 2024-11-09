#include "parser.h"
#include <iostream>

int main(int argc, char **argv) {
  SyntacticParser parser;
  const auto &[record, success] = parser.Parse(argv[1]);

  if (!success) {
    std::cout << "NOT OK\n";
    return 1;
  }

  for (auto entry : record) {
    if (entry.type == Entry::EntryType::COMMAND) {
      Entry::Command command = std::get<Entry::Command>(entry.data);
      std::string cmd;
      switch (command) {
      case Entry::Command::INPUT:
        cmd = "INPUT";
        break;
      case Entry::Command::OUTPUT:
        cmd = "OUTPUT";
        break;
      case Entry::Command::CMPE:
        cmd = "CMPE";
        break;
      case Entry::Command::CMPNE:
        cmd = "CMPNE";
        break;
      case Entry::Command::CMPL:
        cmd = "CMPL";
        break;
      case Entry::Command::CMPG:
        cmd = "CMPG";
        break;
      case Entry::Command::MOV:
        cmd = "MOV";
        break;
      case Entry::Command::OR:
        cmd = "OR";
        break;
      case Entry::Command::AND:
        cmd = "AND";
        break;
      case Entry::Command::ADD:
        cmd = "ADD";
        break;
      case Entry::Command::SUB:
        cmd = "SUB";
        break;
      case Entry::Command::MUL:
        cmd = "MUL";
        break;
      case Entry::Command::DIV:
        cmd = "DIV";
        break;
      case Entry::Command::JZ:
        cmd = "JZ";
        break;
      case Entry::Command::JMP:
        cmd = "JMP";
        break;
      case Entry::Command::BRACKET:
        cmd = "BRACKET";
        break;
      }
      std::cout << cmd << " ";
    } else if (entry.type == Entry::EntryType::VARIABLE) {
      std::cout << std::get<std::string>(entry.data) << " ";
    } else if (entry.type == Entry::EntryType::CONSTANT ||
               entry.type == Entry::EntryType::INSTRUCTION_POINTER) {
      std::cout << std::get<int>(entry.data) << " ";
    }
  }
  std::cout << std::endl;
}
