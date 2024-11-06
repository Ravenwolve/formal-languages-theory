#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct Lexeme {
  enum Type {
    IF,
    THEN,
    END,
    ELSEIF,
    ELSE,
    AND,
    OR,
    RELATION,
    ARITHMETIC_SIMPLE,
    ARITHMETIC_DIFICULT,
    ASSIGNMENT,
    INPUT,
    OUTPUT,
    BRACKET,
    SEPARATOR,
    UNDEFINED
  } type;
  enum Category { KEYWORD, SPECIAL_SYMBOL, IDENTIFIER, CONSTANT } category;
  std::string value;
};

const std::unordered_map<std::string, Lexeme> TERMINAL_LEXEMES{
    {"if", {Lexeme::Type::IF, Lexeme::Category::KEYWORD, "if"}},
    {"then", {Lexeme::Type::THEN, Lexeme::Category::KEYWORD, "then"}},
    {"end", {Lexeme::Type::END, Lexeme::Category::KEYWORD, "end"}},
    {"elseif", {Lexeme::Type::ELSEIF, Lexeme::Category::KEYWORD, "elseif"}},
    {"else", {Lexeme::Type::ELSE, Lexeme::Category::KEYWORD, "else"}},
    {"and", {Lexeme::Type::AND, Lexeme::Category::KEYWORD, "and"}},
    {"or", {Lexeme::Type::OR, Lexeme::Category::KEYWORD, "or"}},
    {"<", {Lexeme::Type::RELATION, Lexeme::Category::SPECIAL_SYMBOL, "<"}},
    {">", {Lexeme::Type::RELATION, Lexeme::Category::SPECIAL_SYMBOL, ">"}},
    {"==", {Lexeme::Type::RELATION, Lexeme::Category::SPECIAL_SYMBOL, "=="}},
    {"<>", {Lexeme::Type::RELATION, Lexeme::Category::SPECIAL_SYMBOL, "<>"}},
    {"+",
     {Lexeme::Type::ARITHMETIC_SIMPLE, Lexeme::Category::SPECIAL_SYMBOL, "+"}},
    {"-",
     {Lexeme::Type::ARITHMETIC_SIMPLE, Lexeme::Category::SPECIAL_SYMBOL, "-"}},
    {"*",
     {Lexeme::Type::ARITHMETIC_DIFICULT, Lexeme::Category::SPECIAL_SYMBOL,
      "*"}},
    {"/",
     {Lexeme::Type::ARITHMETIC_DIFICULT, Lexeme::Category::SPECIAL_SYMBOL,
      "/"}},
    {"=", {Lexeme::Type::ASSIGNMENT, Lexeme::Category::SPECIAL_SYMBOL, "="}},
    {"input", {Lexeme::Type::INPUT, Lexeme::Category::KEYWORD, "input"}},
    {"output", {Lexeme::Type::OUTPUT, Lexeme::Category::KEYWORD, "output"}},
    {"(", {Lexeme::Type::BRACKET, Lexeme::Category::SPECIAL_SYMBOL, "("}},
    {")", {Lexeme::Type::BRACKET, Lexeme::Category::SPECIAL_SYMBOL, ")"}},
    {";", {Lexeme::Type::SEPARATOR, Lexeme::Category::SPECIAL_SYMBOL, ";"}},
};
