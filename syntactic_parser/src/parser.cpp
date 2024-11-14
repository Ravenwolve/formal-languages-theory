#include "parser.h"
#include <iostream>

using Iterator = std::vector<Lexeme>::iterator;

static void error(Iterator iter, const Iterator end,
                  const std::string &reason) {
  std::cerr << "syntax error: " << reason << '\n';
  for (auto item = iter; item != end; ++item) {
    std::cerr << item->value << ' ';
  }
  std::cerr << "\033[1;31m" << end->value << "\033[0m\n";
}

std::tuple<Iterator, bool> SyntacticParser::IfStatement(Iterator begin,
                                                        Iterator end) {
  if (!If(begin)) {
    return {begin, false};
  }

  const auto &[endOfLogExpr, success1] = LogExpr(begin + 1, end);
  if (!success1) {
    error(begin, endOfLogExpr + 1,
          "expected logical expression as condition in 'if' statement");
    exit(1);
  }

  if (!Then(endOfLogExpr + 1)) {
    error(begin, endOfLogExpr + 1,
          "expected keyword 'then' after condition of 'if' statement");
    exit(1);
  }

  const auto &[endOfStatement, success2] = Statement(endOfLogExpr + 2, end);
  if (!success2) {
    error(begin, endOfStatement, "expected statement in 'if' body");
    exit(1);
  }

  const auto &[endOfOptionalAlterIfStatement, success3] =
      AlterIfStatement(endOfStatement + 1, end);

  if (!End(endOfOptionalAlterIfStatement + 1)) {
    error(begin, endOfOptionalAlterIfStatement + 1,
          "expected keyword 'end' after 'if' body");
    exit(1);
  }
  return {endOfOptionalAlterIfStatement + 1, true};
}

std::tuple<Iterator, bool> SyntacticParser::AlterIfStatement(Iterator begin,
                                                             Iterator end) {
  if (ElseIf(begin)) {
    const auto &[endOfLogExpr, success1] = LogExpr(begin + 1, end);
    if (!success1) {
      error(begin + 1, endOfLogExpr,
            "expected logical expression as condition in 'elseif' statement");
      exit(1);
    }
    if (!Then(endOfLogExpr + 1)) {
      error(begin, endOfLogExpr + 1,
            "expected keyword 'then' after condition of 'elseif' statement");
      exit(1);
    }
    const auto &[endOfStatement, success2] = Statement(endOfLogExpr + 2, end);
    if (!success2) {
      error(begin, endOfStatement, "expected statement in 'if' body");
      exit(1);
    }
    const auto &[endOfAlterIfStatement, success3] =
        AlterIfStatement(endOfStatement + 1, end);
    return {endOfAlterIfStatement, true};
  } else if (Else(begin)) {
    const auto &[endOfStatement, success] = Statement(begin + 1, end);
    if (!success) {
      error(begin + 1, endOfStatement, "expected statement in 'else' body");
      exit(1);
    }
    return {endOfStatement, true};
  }

  return {begin, false};
}

std::tuple<Iterator, bool> SyntacticParser::LogExpr(Iterator begin,
                                                    Iterator end) {
  const auto &[endOfLogExprInner, success] = LogExprInner(begin, end);
  if (!success) {
    return {begin, false};
  }

  Iterator endOfLastLogExprInner = endOfLogExprInner;
  while (LogOp1(endOfLastLogExprInner + 1)) {
    const auto &[endOfLogExprInner, success] =
        LogExprInner(endOfLastLogExprInner + 2, end);
    if (!success) {
      error(begin, endOfLogExprInner,
            "expected relation expression in logical expression");
      exit(1);
    }
    endOfLastLogExprInner = endOfLogExprInner;
  }
  return {endOfLastLogExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::LogExprInner(Iterator begin,
                                                         Iterator end) {
  const auto &[endOfRelExpr, success] = RelExpr(begin, end);
  if (!success) {
    return {begin, false};
  }

  Iterator endOfLastRelExpr = endOfRelExpr;
  while (LogOp2(endOfLastRelExpr + 1)) {
    const auto &[endOfRelExpr, success] = RelExpr(endOfLastRelExpr + 2, end);
    if (!success) {
      error(begin, endOfRelExpr,
            "expected relation expression in logical expression");
      exit(1);
    }
    endOfLastRelExpr = endOfRelExpr;
  }
  return {endOfLastRelExpr, true};
}

std::tuple<Iterator, bool> SyntacticParser::RelExpr(Iterator begin,
                                                    Iterator end) {
  if (!Operand(begin)) {
    return {begin, false};
  }

  if (RelOp(begin + 1)) {
    if (!Operand(begin + 2)) {
      error(begin, begin + 2, "expected operand in relation expression");
      exit(1);
    }
    return {begin + 2, true};
  }
  return {begin, true};
}

bool SyntacticParser::RelOp(Iterator begin) {
  return begin->type == Lexeme::Type::RELATION;
}

std::tuple<Iterator, bool> SyntacticParser::Statement(Iterator begin,
                                                      Iterator end) {
  const auto &[endOfInstruction, success] = Instruction(begin, end);
  if (!success) {
    return {begin, false};
  }

  Iterator endOfLastInstruction = endOfInstruction;
  while (Semicolon(endOfLastInstruction + 1)) {
    const auto &[endOfInstruction, success] =
        Instruction(endOfLastInstruction + 2, end);
    if (!success) {
      error(begin, endOfInstruction, "expected intruction after semicolon");
      exit(1);
    }
    endOfLastInstruction = endOfInstruction;
  }
  return {endOfLastInstruction, true};
}

std::tuple<Iterator, bool> SyntacticParser::Instruction(Iterator begin,
                                                        Iterator end) {
  if (Identifier(begin)) {
    if (AssignmentOp(begin + 1)) {
      const auto &[endOfArithExpr, success] = ArithExpr(begin + 2, end);
      if (!success) {
        error(begin, endOfArithExpr,
              "expected arithmetic expression after assignment operator");
        exit(1);
      }
      return {endOfArithExpr, true};
    }
  } else if (InputOp(begin)) {
    if (!Identifier(begin + 1)) {
      error(begin, begin + 1, "expected identifier after input keyword");
      exit(1);
    }
    return {begin + 1, true};
  } else if (OutputOp(begin)) {
    if (!Operand(begin + 1)) {
      error(begin, begin + 1, "expected operand after output keyword");
      exit(1);
    }
    return {begin + 1, true};
  }

  return {begin, false};
}

std::tuple<Iterator, bool> SyntacticParser::ArithExpr(Iterator begin,
                                                      Iterator end) {
  const auto &[endOfArithExprInner, success] = ArithExprInner(begin, end);
  if (!success) {
    return {begin, false};
  }

  Iterator endOfLastArithExprInner = endOfArithExprInner;
  while (ArithOp1(endOfLastArithExprInner + 1)) {
    const auto &[endOfArithExprInner, success] =
        ArithExprInner(endOfLastArithExprInner + 2, end);
    if (!success) {
      error(begin, endOfLastArithExprInner,
            "expected arithmetic expression after arithmetic operator");
      exit(1);
    }
    endOfLastArithExprInner = endOfArithExprInner;
  }
  return {endOfLastArithExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::ArithExprInner(Iterator begin,
                                                           Iterator end) {
  const auto &[endOfScopedArithExpr, success] = ScopedArithExpr(begin, end);
  if (!success) {
    return {begin, false};
  }

  Iterator endOfLastArithExpr = endOfScopedArithExpr;
  while (ArithOp2(endOfLastArithExpr + 1)) {
    const auto &[endOfArithExprInner, success] =
        ArithExprInner(endOfLastArithExpr + 2, end);
    if (!success) {
      error(begin, endOfLastArithExpr,
            "expected arithmetic expression after arithmetic operator");
      exit(1);
    }
    endOfLastArithExpr = endOfArithExprInner;
  }
  return {endOfLastArithExpr, true};
}

std::tuple<Iterator, bool> SyntacticParser::ScopedArithExpr(Iterator begin,
                                                            Iterator end) {
  if (Operand(begin)) {
    return {begin, true};
  }

  if (!OpeningParenthesis(begin)) {
    return {begin, false};
  }

  const auto &[endOfArithExpr, success] = ArithExpr(begin + 1, end);
  if (!success) {
    error(begin, endOfArithExpr, "expected arithmetic expression after '(");
    exit(1);
  }

  if (!ClosingParenthesis(endOfArithExpr + 1)) {
    error(begin, endOfArithExpr, "expected ')' after arithmetic expression");
    exit(1);
  }

  return {endOfArithExpr + 1, true};
}

bool SyntacticParser::Operand(Iterator begin) {
  return Identifier(begin) || Constant(begin);
}

bool SyntacticParser::Identifier(Iterator begin) {
  return begin->category == Lexeme::Category::IDENTIFIER;
}

bool SyntacticParser::Constant(Iterator begin) {
  return begin->category == Lexeme::Category::CONSTANT;
}

bool SyntacticParser::ArithOp1(Iterator begin) {
  return begin->type == Lexeme::Type::ARITHMETIC_SIMPLE;
}

bool SyntacticParser::ArithOp2(Iterator begin) {
  return begin->type == Lexeme::Type::ARITHMETIC_DIFICULT;
}

bool SyntacticParser::LogOp1(Iterator begin) {
  return begin->type == Lexeme::Type::OR;
}

bool SyntacticParser::LogOp2(Iterator begin) {
  return begin->type == Lexeme::Type::AND;
}

bool SyntacticParser::OpeningParenthesis(Iterator begin) {
  return begin->type == Lexeme::Type::BRACKET && begin->value == "(";
}

bool SyntacticParser::ClosingParenthesis(Iterator begin) {
  return begin->type == Lexeme::Type::BRACKET && begin->value == ")";
}

bool SyntacticParser::Semicolon(Iterator begin) {
  return begin->type == Lexeme::Type::SEPARATOR;
}

bool SyntacticParser::AssignmentOp(Iterator begin) {
  return begin->type == Lexeme::Type::ASSIGNMENT;
}

bool SyntacticParser::InputOp(Iterator begin) {
  return begin->type == Lexeme::Type::INPUT;
}

bool SyntacticParser::OutputOp(Iterator begin) {
  return begin->type == Lexeme::Type::OUTPUT;
}

bool SyntacticParser::If(Iterator begin) {
  return begin->type == Lexeme::Type::IF;
}

bool SyntacticParser::Then(Iterator begin) {
  return begin->type == Lexeme::Type::THEN;
}

bool SyntacticParser::End(Iterator begin) {
  return begin->type == Lexeme::Type::END;
}

bool SyntacticParser::ElseIf(Iterator begin) {
  return begin->type == Lexeme::Type::ELSEIF;
}

bool SyntacticParser::Else(Iterator begin) {
  return begin->type == Lexeme::Type::ELSE;
}

bool SyntacticParser::Parse(const std::string &text) {
  auto lexemes = lexer.Analyse(text);

  auto begin = lexemes.begin();
  auto end = lexemes.end();
  const auto &[endOfIfStatement, success] = IfStatement(begin, end);

  return success;
}
