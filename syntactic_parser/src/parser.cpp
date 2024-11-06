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

static bool Separator(std::vector<Lexeme>::iterator &iter,
                      const std::vector<Lexeme>::iterator &end) {
  return iter->type == Lexeme::Type::SEPARATOR;
}

static bool RelationOperator(std::vector<Lexeme>::iterator &iter,
                             const std::vector<Lexeme>::iterator &end) {
  return iter->type == Lexeme::Type::RELATION;
}

static bool LogicalOperator(std::vector<Lexeme>::iterator &iter,
                            const std::vector<Lexeme>::iterator &end) {
  if (iter->category != Lexeme::Category::KEYWORD ||
      (iter->type != Lexeme::Type::AND && iter->type != Lexeme::Type::OR)) {
    return false;
  }

  return true;
}

static bool ArithmeticOperator(std::vector<Lexeme>::iterator &iter,
                               const std::vector<Lexeme>::iterator &end) {
  if (iter->category != Lexeme::Category::SPECIAL_SYMBOL ||
      iter->type != Lexeme::Type::ARITHMETIC_SIMPLE &&
          iter->type != Lexeme::Type::ARITHMETIC_DIFICULT) {
    return false;
  }

  return true;
}

static bool Operand(std::vector<Lexeme>::iterator &iter,
                    const std::vector<Lexeme>::iterator &end) {
  if (iter->category != Lexeme::Category::CONSTANT &&
      iter->category != Lexeme::Category::IDENTIFIER) {
    return false;
  }

  return true;
}

static bool OpenedBracket(std::vector<Lexeme>::iterator &iter,
                          const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::BRACKET || iter->value != "(") {
    return false;
  }

  return true;
}

static bool ClosedBracket(std::vector<Lexeme>::iterator &iter,
                          const std::vector<Lexeme>::iterator &end) {
  if (iter->type != Lexeme::Type::BRACKET || iter->value != ")") {
    return false;
  }

  return true;
}

static std::optional<bool>
RelationshipExpression(std::vector<Lexeme>::iterator &iter,
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

static std::optional<bool>
ArithmeticExpression(std::vector<Lexeme>::iterator &iter,
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

static std::optional<bool>
LogicalExpression(std::vector<Lexeme>::iterator &iter,
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

static std::optional<bool> Statement(std::vector<Lexeme>::iterator &iter,
                                     const std::vector<Lexeme>::iterator &end) {
  auto begin = iter;
  if (iter->category == Lexeme::Category::IDENTIFIER) {
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
      if ((++iter)->category != Lexeme::Category::IDENTIFIER) {
        error(begin, iter, "expected identifier after 'input'");
        return std::nullopt;
      }
    } else if (iter->type == Lexeme::Type::OUTPUT) {
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

static std::optional<bool>
ConditionalStatement(std::vector<Lexeme>::iterator &iter,
                     const std::vector<Lexeme>::iterator &end,
                     Lexeme::Type type = Lexeme::Type::IF) {
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
    std::optional<bool> optionalStatement;
    do {
      optionalStatement = Statement(++iter, end);
      if (!optionalStatement.has_value()) {
        return std::nullopt;
      }
    } while (optionalStatement.value());
    if (iter->type == Lexeme::Type::ELSEIF) {
      return ConditionalStatement(iter, end, Lexeme::Type::ELSEIF);
    } else if (iter->type == Lexeme::Type::ELSE) {
      do {
        optionalStatement = Statement(++iter, end);
        if (!optionalStatement.has_value()) {
          return std::nullopt;
        }
      } while (optionalStatement.value());
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

bool SyntacticParser::Parse(const std::string &text) {
  auto lexemes = lexer.Analyse(text);

  auto begin = lexemes.begin();
  auto end = lexemes.end();
  auto optionalConditionalStatement = ConditionalStatement(begin, end);

  return optionalConditionalStatement.has_value() &&
         optionalConditionalStatement.value();
}
