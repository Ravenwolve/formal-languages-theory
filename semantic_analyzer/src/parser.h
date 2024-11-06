#pragma once

#include "../../lexical_analyzer/src/analyzer.h"
#include "entrycollection.h"
#include <optional>

class SyntacticParser {
public:
  std::optional<std::vector<Entry>> Parse(const std::string &text);

private:
  bool Separator(std::vector<Lexeme>::iterator &iter,
                 const std::vector<Lexeme>::iterator &end);
  bool Operand(std::vector<Lexeme>::iterator &iter,
               const std::vector<Lexeme>::iterator &end);
  bool RelationOperator(std::vector<Lexeme>::iterator &iter,
                        const std::vector<Lexeme>::iterator &end);
  bool LogicalOperator(std::vector<Lexeme>::iterator &iter,
                       const std::vector<Lexeme>::iterator &end);
  bool ArithmeticOperator(std::vector<Lexeme>::iterator &iter,
                          const std::vector<Lexeme>::iterator &end);
  bool OpenedBracket(std::vector<Lexeme>::iterator &iter,
                     const std::vector<Lexeme>::iterator &end);
  bool ClosedBracket(std::vector<Lexeme>::iterator &iter,
                     const std::vector<Lexeme>::iterator &end);
  std::optional<bool>
  RelationshipExpression(std::vector<Lexeme>::iterator &iter,
                         const std::vector<Lexeme>::iterator &end);
  std::optional<bool>
  LogicalExpression(std::vector<Lexeme>::iterator &iter,
                    const std::vector<Lexeme>::iterator &end);
  std::optional<bool>
  ArithmeticExpression(std::vector<Lexeme>::iterator &iter,
                       const std::vector<Lexeme>::iterator &end);
  std::optional<bool> Statement(std::vector<Lexeme>::iterator &iter,
                                const std::vector<Lexeme>::iterator &end);
  std::optional<bool>
  ConditionalStatement(std::vector<Lexeme>::iterator &iter,
                       const std::vector<Lexeme>::iterator &end,
                       Lexeme::Type type = Lexeme::Type::IF);

public:
  LexicalAnalyzer lexer;
  PolishInverseRecord sequence;
};
