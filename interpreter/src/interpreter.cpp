#include "interpreter.h"
#include <iostream>

void Interpreter::Debug(Entry entry) {
  std::cerr << "Current entry: ";
  if (entry.type == Entry::EntryType::COMMAND) {
    auto command = std::get<Entry::Command>(entry.data);
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
      cmd = "SET";
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
    }
    std::cerr << cmd;
  } else if (entry.type == Entry::EntryType::CONSTANT ||
             entry.type == Entry::EntryType::INSTRUCTION_POINTER) {
    std::cerr << std::get<int>(entry.data);
  } else if (entry.type == Entry::EntryType::VARIABLE) {
    std::cerr << std::get<std::string>(entry.data);
  }
  std::cerr << std::endl;

  auto stackCopy = stack;
  std::cerr << "Stack (HEAD->TAIL): ";
  while (!stackCopy.empty()) {
    try {
      std::cerr << std::get<std::string>(stackCopy.top()) << " ";
    } catch (...) {
      std::cerr << std::get<int>(stackCopy.top()) << " ";
    }
    stackCopy.pop();
  }
  std::cerr << std::endl;

  for (const auto &[var, value] : variables) {
    std::cerr << var << "=" << value << "; ";
  }
  std::cerr << std::endl;
}

int Interpreter::Value(const std::variant<int, std::string> &operand) {
  int value;
  try {
    value = std::get<int>(stack.top());
  } catch (...) {
    auto var = std::get<std::string>(stack.top());
    if (!variables.contains(var)) {
      variables[var] = 0;
    }
    value = variables[var];
  }
  return value;
}

bool Interpreter::Interprete(const std::string &text) {
  const auto &[record, success] = parser.Parse(text);
  if (!success) {
    return false;
  }
  auto entries = std::move(record);

  for (auto iter = entries.begin(); iter != entries.end(); ++iter) {
    Debug(*iter);
    if (iter->type == Entry::EntryType::CONSTANT ||
        iter->type == Entry::EntryType::INSTRUCTION_POINTER) {
      int value = std::get<int>(iter->data);
      stack.push(value);
    } else if (iter->type == Entry::EntryType::VARIABLE) {
      auto var = std::get<std::string>(iter->data);
      stack.push(var);
    } else if (iter->type == Entry::EntryType::COMMAND) {
      auto cmd = std::get<Entry::Command>(iter->data);
      switch (cmd) {
      case Entry::Command::INPUT: {
        auto op = std::move(std::get<std::string>(stack.top()));
        stack.pop();
        std::cin >> variables[op];
        break;
      }
      case Entry::Command::OUTPUT:
        std::cout << Value(stack.top());
        stack.pop();
        break;
      case Entry::Command::CMPE: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 == op2);
        break;
      }
      case Entry::Command::CMPNE: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 != op2);
        break;
      }
      case Entry::Command::CMPL: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 < op2);
        break;
      }
      case Entry::Command::CMPG: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 > op2);
        break;
      }
      case Entry::Command::MOV: {
        int op2 = Value(stack.top());
        stack.pop();
        auto op1 = std::move(std::get<std::string>(stack.top()));
        stack.pop();
        variables[op1] = op2;
        break;
      }
      case Entry::Command::OR: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 || op2);
        break;
      }
      case Entry::Command::AND: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 && op2);
        break;
      }
      case Entry::Command::ADD: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 + op2);
        break;
      }
      case Entry::Command::SUB: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 - op2);
        break;
      }
      case Entry::Command::MUL: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 * op2);
        break;
      }
      case Entry::Command::DIV: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        stack.push(op1 / op2);
        break;
      }
      case Entry::Command::JZ: {
        int op2 = Value(stack.top());
        stack.pop();
        int op1 = Value(stack.top());
        stack.pop();
        if (!op1) {
          iter = entries.begin() + op2 - 1;
        }
        break;
      }
      case Entry::Command::JMP: {
        int op = Value(stack.top());
        stack.pop();
        iter = entries.begin() + op - 1;
      }
      }
    }
  }

  Debug({Entry::EntryType::COMMAND, Entry::Command::BRACKET});

  return true;
}
