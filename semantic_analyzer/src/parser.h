#pragma once

#include "../../lexical_analyzer/src/analyzer.h"
#include "entrycollection.h"

class SyntacticParser {
public:
  std::tuple<std::vector<Entry>, bool> Parse(const std::string &text);

private:
  using Iterator = std::vector<Lexeme>::iterator;
  auto IfStatement(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto LogExpr(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto LogExprInner(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto RelExpr(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto RelOp(Iterator begin) -> bool;
  auto Statement(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto Instruction(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto ArithExpr(Iterator begin, Iterator end) -> std::tuple<Iterator, bool>;
  auto ArithExprInner(Iterator begin,
                      Iterator end) -> std::tuple<Iterator, bool>;
  auto ScopedArithExpr(Iterator begin,
                       Iterator end) -> std::tuple<Iterator, bool>;
  auto ArithOp1(Iterator begin) -> bool;
  auto ArithOp2(Iterator begin) -> bool;
  auto LogOp1(Iterator begin) -> bool;
  auto LogOp2(Iterator begin) -> bool;
  auto Operand(Iterator begin) -> bool;
  auto Identifier(Iterator begin) -> bool;
  auto Constant(Iterator begin) -> bool;
  auto OpeningParenthesis(Iterator begin) -> bool;
  auto ClosingParenthesis(Iterator begin) -> bool;
  auto Semicolon(Iterator begin) -> bool;
  auto AssignmentOp(Iterator begin) -> bool;
  auto InputOp(Iterator begin) -> bool;
  auto OutputOp(Iterator begin) -> bool;
  auto If(Iterator begin) -> bool;
  auto Then(Iterator begin) -> bool;
  auto End(Iterator begin) -> bool;
  auto ElseIf(Iterator begin) -> bool;
  auto Else(Iterator begin) -> bool;

public:
  LexicalAnalyzer lexer;
  PolishInverseRecord sequence;
};
