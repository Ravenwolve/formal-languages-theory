#include "interpreter.h"
#include <iostream>

int main(int argc, char **argv) {
  Interpreter interpreter;
  std::cout << (interpreter.Interprete(argv[1]) ? "\nOK\n" : "\nNOT OK\n");
}
