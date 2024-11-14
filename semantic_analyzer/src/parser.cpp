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

  size_t indexOfJzOp2 = entries.size();
  entries.emplace_back(Entry::EntryType::INSTRUCTION_POINTER, -1);
  entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::JZ);

  const auto &[endOfStatement, success2] = Statement(endOfLogExpr + 2, end);
  if (!success2) {
    error(begin, endOfStatement, "expected statement in 'if' body");
    exit(1);
  }

  entries.emplace_back(Entry::EntryType::INSTRUCTION_POINTER, -1);
  entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::JMP);
  entries[indexOfJzOp2].data = static_cast<int>(entries.size());

  const auto &[endOfOptionalAlterIfStatement, success3] =
      AlterIfStatement(endOfStatement + 1, end);

  if (!End(endOfOptionalAlterIfStatement + 1)) {
    error(begin, endOfOptionalAlterIfStatement + 1,
          "expected keyword 'end' after 'if' body");
    exit(1);
  }

  for (Entry &entry : entries) {
    if (entry.type == Entry::EntryType::INSTRUCTION_POINTER &&
        std::get<int>(entry.data) == -1) {
      entry.data = static_cast<int>(entries.size());
    }
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

    size_t indexOfJzOp2 = entries.size();
    entries.emplace_back(Entry::EntryType::INSTRUCTION_POINTER, -1);
    entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::JZ);

    const auto &[endOfStatement, success2] = Statement(endOfLogExpr + 2, end);
    if (!success2) {
      error(begin, endOfStatement, "expected statement in 'if' body");
      exit(1);
    }

    entries.emplace_back(Entry::EntryType::INSTRUCTION_POINTER, -1);
    entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::JMP);
    entries[indexOfJzOp2].data = static_cast<int>(entries.size());

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

  const auto &[endOfLogExprTail, success2] =
      LogExprTail(endOfLogExprInner + 1, end);
  return {success2 ? endOfLogExprTail : endOfLogExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::LogExprTail(Iterator begin,
                                                        Iterator end) {
  if (!LogOp1(begin)) {
    return {begin, false};
  }

  const auto &[endOfLogExprInner, success] = LogExprInner(begin + 1, end);
  if (!success) {
    error(begin, endOfLogExprInner,
          "expected relation expression in logical expression");
    exit(1);
  }

  entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::OR);

  const auto &[endOfLogExprTail, success2] =
      LogExprTail(endOfLogExprInner + 1, end);
  return {success2 ? endOfLogExprTail : endOfLogExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::LogExprInner(Iterator begin,
                                                         Iterator end) {
  const auto &[endOfRelExpr, success1] = RelExpr(begin, end);
  if (!success1) {
    return {begin, false};
  }

  const auto &[endOfLogExprInnerTail, success2] =
      LogExprInnerTail(endOfRelExpr + 1, end);
  return {success2 ? endOfLogExprInnerTail : endOfRelExpr, true};
}

std::tuple<Iterator, bool> SyntacticParser::LogExprInnerTail(Iterator begin,
                                                             Iterator end) {
  if (!LogOp2(begin)) {
    return {begin, false};
  }

  const auto &[endOfRelExpr, success] = RelExpr(begin + 1, end);
  if (!success) {
    error(begin, endOfRelExpr,
          "expected relation expression in logical expression");
    exit(1);
  }

  entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::AND);

  const auto &[endOfLogExprInnerTail, success2] =
      LogExprInnerTail(endOfRelExpr + 1, end);
  return {success2 ? endOfLogExprInnerTail : endOfRelExpr, true};
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
    if ((begin + 1)->value == ">") {
      entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::CMPG);
    } else if ((begin + 1)->value == "<") {
      entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::CMPL);
    } else if ((begin + 1)->value == "==") {
      entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::CMPE);
    } else if ((begin + 1)->value == "<>") {
      entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::CMPNE);
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
      entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::MOV);
      return {endOfArithExpr, true};
    }
  } else if (InputOp(begin)) {
    if (!Identifier(begin + 1)) {
      error(begin, begin + 1, "expected identifier after input keyword");
      exit(1);
    }
    entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::INPUT);
    return {begin + 1, true};
  } else if (OutputOp(begin)) {
    if (!Operand(begin + 1)) {
      error(begin, begin + 1, "expected operand after output keyword");
      exit(1);
    }
    entries.emplace_back(Entry::EntryType::COMMAND, Entry::Command::OUTPUT);
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

  const auto &[endOfArithExprTail, success2] =
      ArithExprTail(endOfArithExprInner + 1, end);
  return {success2 ? endOfArithExprTail : endOfArithExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::ArithExprTail(Iterator begin,
                                                          Iterator end) {
  if (!ArithOp1(begin)) {
    return {begin, false};
  }

  const auto &[endOfArithExprInner, success] = ArithExprInner(begin + 1, end);
  if (!success) {
    error(begin, endOfArithExprInner,
          "expected operand in arithmetic expression");
    exit(1);
  }

  entries.emplace_back(Entry::EntryType::COMMAND, begin->value == "+"
                                                      ? Entry::Command::ADD
                                                      : Entry::Command::SUB);

  const auto &[endOfArithExprTail, success2] =
      ArithExprTail(endOfArithExprInner + 1, end);
  return {success2 ? endOfArithExprTail : endOfArithExprInner, true};
}

std::tuple<Iterator, bool> SyntacticParser::ArithExprInner(Iterator begin,
                                                           Iterator end) {
  const auto &[endOfArithUnit, success1] = ArithUnit(begin, end);
  if (!success1) {
    return {begin, false};
  }

  const auto &[endOfArithExprInnerTail, success2] =
      ArithExprInnerTail(endOfArithUnit + 1, end);
  return {success2 ? endOfArithExprInnerTail : endOfArithUnit, true};
}

std::tuple<Iterator, bool> SyntacticParser::ArithExprInnerTail(Iterator begin,
                                                               Iterator end) {
  if (!ArithOp2(begin)) {
    return {begin, false};
  }

  const auto &[endOfArithUnit, success] = ArithUnit(begin + 1, end);
  if (!success) {
    error(begin, endOfArithUnit, "expected operand in arithmetic expression");
    exit(1);
  }

  entries.emplace_back(Entry::EntryType::COMMAND, begin->value == "*"
                                                      ? Entry::Command::MUL
                                                      : Entry::Command::DIV);

  const auto &[endOfArithExprInnerTail, success2] =
      ArithExprInnerTail(endOfArithUnit + 1, end);
  return {success2 ? endOfArithExprInnerTail : endOfArithUnit, true};
}

std::tuple<Iterator, bool> SyntacticParser::ArithUnit(Iterator begin,
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
    error(begin, endOfArithExpr + 1,
          "expected ')' after arithmetic expression");
    exit(1);
  }

  return {endOfArithExpr + 1, true};
}

bool SyntacticParser::Operand(Iterator begin) {
  return Identifier(begin) || Constant(begin);
}

bool SyntacticParser::Identifier(Iterator begin) {
  if (begin->category != Lexeme::Category::IDENTIFIER) {
    return false;
  }

  entries.emplace_back(Entry::EntryType::VARIABLE, begin->value);
  return true;
}

bool SyntacticParser::Constant(Iterator begin) {
  if (begin->category != Lexeme::Category::CONSTANT) {
    return false;
  }

  entries.emplace_back(Entry::EntryType::CONSTANT, std::stoi(begin->value));
  return true;
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

std::tuple<std::vector<Entry>, bool>
SyntacticParser::Parse(const std::string &text) {
  auto lexemes = lexer.Analyse(text);

  auto begin = lexemes.begin();
  auto end = lexemes.end();
  const auto &[endOfIfStatement, success] = IfStatement(begin, end);

  return {entries, success};
}
