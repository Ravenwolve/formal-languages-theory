#include "parser.h"
#include <iostream>
#include <optional>

static void error(std::vector<Lexeme>::iterator &iter,
                  const std::vector<Lexeme>::iterator &end,
                  const std::string &reason) {
  std::cerr << "syntax error: " << reason << '\n';
  for (auto item = iter; item != end; ++item) {
    std::cerr << item->value << ' ';
  }
  std::cerr << "\033[1;31m" << end->value << "\033[0m\n";
}

bool SyntacticParser::Separator(std::vector<Lexeme>::iterator &iter,
                                const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::SEPARATOR) {
    return false;
  }

  sequence.Final();
  return true;
}

bool SyntacticParser::RelationOperator(
    std::vector<Lexeme>::iterator &iter,
    const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::RELATION) {
    return false;
  }

  if (iter->value == ">") {
    sequence.Push(Entry::Command::CMPG);
  } else if (iter->value == "<") {
    sequence.Push(Entry::Command::CMPL);
  } else if (iter->value == "==") {
    sequence.Push(Entry::Command::CMPE);
  } else if (iter->value == "<>") {
    sequence.Push(Entry::Command::CMPNE);
  }

  return true;
}

bool SyntacticParser::LogicalOperator(
    std::vector<Lexeme>::iterator &iter,
    const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::AND && iter->type != Lexeme::Type::OR) {
    return false;
  }

  if (iter->value == "and") {
    sequence.Push(Entry::Command::AND);
  } else if (iter->value == "or") {
    sequence.Push(Entry::Command::OR);
  }

  return true;
}

bool SyntacticParser::ArithmeticOperator(
    std::vector<Lexeme>::iterator &iter,
    const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::ARITHMETIC_SIMPLE &&
      iter->type != Lexeme::Type::ARITHMETIC_DIFICULT) {
    return false;
  }

  if (iter->value == "+") {
    sequence.Push(Entry::Command::ADD);
  } else if (iter->value == "-") {
    sequence.Push(Entry::Command::SUB);
  } else if (iter->value == "*") {
    sequence.Push(Entry::Command::MUL);
  } else if (iter->value == "/") {
    sequence.Push(Entry::Command::DIV);
  }

  return true;
}

bool SyntacticParser::Operand(std::vector<Lexeme>::iterator &iter,
                              const std::vector<Lexeme>::iterator &end) {
  if (iter->category != Lexeme::Category::CONSTANT &&
      iter->category != Lexeme::Category::IDENTIFIER) {
    return false;
  }

  if (iter->category == Lexeme::Category::CONSTANT) {
    sequence.PushConstant(iter->value);
  } else if (iter->category == Lexeme::Category::IDENTIFIER) {
    sequence.PushVariable(iter->value);
  }

  return true;
}

bool SyntacticParser::OpenedBracket(std::vector<Lexeme>::iterator &iter,
                                    const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::BRACKET || iter->value != "(") {
    return false;
  }

  sequence.PushOpeningBracket();
  return true;
}

bool SyntacticParser::ClosedBracket(std::vector<Lexeme>::iterator &iter,
                                    const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::BRACKET || iter->value != ")") {
    return false;
  }

  sequence.PushClosingBracket();
  return true;
}

std::optional<bool> SyntacticParser::RelationshipExpression(
    std::vector<Lexeme>::iterator &iter,
    const std::vector<Lexeme>::iterator &end) {
  auto begin = iter;
  if (!Operand(iter, end)) {
    return false;
  }
  if (!RelationOperator(++iter, end)) {
    return false;
  }
  if (!Operand(++iter, end)) {
    error(begin, iter, "expected operand in relation expression");
    return std::nullopt;
  }

  return true;
}

std::optional<bool> SyntacticParser::ArithmeticExpression(
    std::vector<Lexeme>::iterator &iter,
    const std::vector<Lexeme>::iterator &end) {
  auto begin = iter;
  int opened_brackets_counter = 0;

  --iter;
  do {
    while (OpenedBracket(++iter, end)) {
      ++opened_brackets_counter;
    }
    if (!Operand(iter, end)) {
      if (opened_brackets_counter) {
        error(begin, iter, "expected operand in arithmetic expression");
        return std::nullopt;
      }
      return false;
    }
    while (ClosedBracket(++iter, end)) {
      --opened_brackets_counter;
    }
  } while (ArithmeticOperator(iter, end));

  if (opened_brackets_counter > 0) {
    error(begin, iter, "expected closing brackets in arithmetic expression");
    return std::nullopt;
  } else if (opened_brackets_counter < 0) {
    error(begin, iter - 1,
          "too many closing brackets in arithmetic expression");
    return std::nullopt;
  }

  return true;
}

std::optional<bool>
SyntacticParser::LogicalExpression(std::vector<Lexeme>::iterator &iter,
                                   const std::vector<Lexeme>::iterator &end) {
  auto begin = iter;
  int opened_brackets_counter = 0;

  --iter;
  do {
    while (OpenedBracket(++iter, end)) {
      ++opened_brackets_counter;
    }
    auto optionalRelationshipExpression = RelationshipExpression(iter, end);
    if (!optionalRelationshipExpression.has_value()) {
      return std::nullopt;
    }
    if (!optionalRelationshipExpression.value()) {
      if (opened_brackets_counter) {
        error(begin, iter, "expected operand in logical expression");
        return std::nullopt;
      }
      return false;
    }
    while (ClosedBracket(++iter, end)) {
      --opened_brackets_counter;
    }
  } while (LogicalOperator(iter, end));

  if (opened_brackets_counter > 0) {
    error(begin, iter, "expected closing brackets in logical expression");
    return std::nullopt;
  } else if (opened_brackets_counter < 0) {
    error(begin, iter - 1, "too many closing brackets in logical expression");
    return std::nullopt;
  }

  return true;
}

std::optional<bool>
SyntacticParser::Statement(std::vector<Lexeme>::iterator &iter,
                           const std::vector<Lexeme>::iterator &end) {
  auto begin = iter;
  if (iter->category == Lexeme::Category::IDENTIFIER) {
    sequence.Push(Entry::Command::MOV);
    sequence.PushVariable(iter->value);
    ++iter;
    if (iter->type != Lexeme::Type::ASSIGNMENT) {
      error(begin, iter, "expected assignment operator after identifier");
      return std::nullopt;
    }
    auto optionalArithmeticExpression = ArithmeticExpression(++iter, end);
    if (!optionalArithmeticExpression.has_value()) {
      return std::nullopt;
    }
    if (!optionalArithmeticExpression.value()) {
      error(begin, iter,
            "expected arithmetic expression after assignment operator");
      return std::nullopt;
    }
    --iter;
  } else if (iter->type == Lexeme::Type::INPUT ||
             iter->type == Lexeme::Type::OUTPUT) {
    if (iter->type == Lexeme::Type::INPUT) {
      sequence.Push(Entry::Command::INPUT);
      if ((++iter)->category != Lexeme::Category::IDENTIFIER) {
        error(begin, iter, "expected identifier after 'input'");
        return std::nullopt;
      }
      sequence.PushVariable(iter->value);
    } else if (iter->type == Lexeme::Type::OUTPUT) {
      sequence.Push(Entry::Command::OUTPUT);
      if (!Operand(++iter, end)) {
        error(begin, iter, "expected operand after 'output'");
        return std::nullopt;
      }
    }
  } else {
    return false;
  }

  if (!Separator(++iter, end)) {
    error(begin, iter, "expected separator in the end of statement");
    return std::nullopt;
  }

  return true;
}

std::optional<bool>
SyntacticParser::ConditionalStatement(std::vector<Lexeme>::iterator &iter,
                                      const std::vector<Lexeme>::iterator &end,
                                      Lexeme::Type type) {
  auto begin = iter;
  if (iter->type == type) {
    auto optionalLogicalExpression = LogicalExpression(++iter, end);
    if (!optionalLogicalExpression.has_value()) {
      return std::nullopt;
    }
    if (!optionalLogicalExpression.value()) {
      error(begin, iter, "expected logical condition");
      return std::nullopt;
    }
    if (iter->type != Lexeme::Type::THEN) {
      error(begin, iter,
            "expected keyword 'then' for opening 'if' body after condition");
      return std::nullopt;
    }
    sequence.Final();
    size_t indexOfNextIfBody = sequence.PushAddress(-1); // fictive address
    sequence.ForcePush(Entry::Command::JZ);
    std::optional<bool> optionalStatement;
    do {
      optionalStatement = Statement(++iter, end);
      if (!optionalStatement.has_value()) {
        return std::nullopt;
      }
    } while (optionalStatement.value());
    size_t indexOfEnd = sequence.PushAddress(-1); // fictive address
    sequence.ForcePush(Entry::Command::JMP);
    if (iter->type == Lexeme::Type::ELSEIF) {
      sequence.RewriteAddress(indexOfNextIfBody, sequence.Size());
      auto optionalConditionalStatement =
          ConditionalStatement(iter, end, Lexeme::Type::ELSEIF);
      if (!optionalConditionalStatement.has_value() |
          !optionalConditionalStatement.value()) {
        return optionalConditionalStatement;
      }
      sequence.RewriteAddress(indexOfEnd, sequence.Size());
      return true;
    } else if (iter->type == Lexeme::Type::ELSE) {
      sequence.RewriteAddress(indexOfNextIfBody, sequence.Size());
      do {
        optionalStatement = Statement(++iter, end);
        if (!optionalStatement.has_value()) {
          return std::nullopt;
        }
      } while (optionalStatement.value());
      sequence.RewriteAddress(indexOfEnd, sequence.Size());
    }
    if (iter->type == Lexeme::Type::END) {
      return true;
    } else {
      error(iter, iter, "expected keyword 'end' for closing 'if' body");
      return std::nullopt;
    }
  }

  return false;
}

std::optional<std::vector<Entry>>
SyntacticParser::Parse(const std::string &text) {
  auto lexemes = lexer.Analyse(text);

  auto begin = lexemes.begin();
  auto end = lexemes.end();
  auto optionalConditionalStatement = ConditionalStatement(begin, end);

  if (!optionalConditionalStatement.has_value() ||
      !optionalConditionalStatement.value()) {
    return std::nullopt;
  }

  return sequence.ToVector();
}
