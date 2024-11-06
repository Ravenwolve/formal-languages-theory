#include "parser.h"
#include <iostream>

int main(int argc, char **argv) {
  SyntacticParser parser;

  if (parser.Parse(argv[1])) {
    std::cout << "OK\n";
  } else {
    std::cout << "NOT OK\n";
  }
}
