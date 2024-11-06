#pragma once

#include "../../lexical_analyzer/src/analyzer.h"

class SyntacticParser {
public:
  bool Parse(const std::string &text);

public:
  LexicalAnalyzer lexer;
};
