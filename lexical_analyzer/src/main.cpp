#include "analyzer.h"
#include <iostream>
#include <unordered_map>

const std::unordered_map<Lexeme::Type, std::string> TYPES{
    {Lexeme::Type::IF, "IF"},
    {Lexeme::Type::THEN, "THEN"},
    {Lexeme::Type::END, "END"},
    {Lexeme::Type::ELSEIF, "ELSEIF"},
    {Lexeme::Type::ELSE, "ELSE"},
    {Lexeme::Type::AND, "AND"},
    {Lexeme::Type::OR, "OR"},
    {Lexeme::Type::RELATION, "RELATION"},
    {Lexeme::Type::ARITHMETIC_SIMPLE, "ARITHMETIC_SIMPLE"},
    {Lexeme::Type::ARITHMETIC_DIFICULT, "ARITHMETIC_DIFICULT"},
    {Lexeme::Type::ASSIGNMENT, "ASSIGNMENT"},
    {Lexeme::Type::INPUT, "INPUT"},
    {Lexeme::Type::OUTPUT, "OUTPUT"},
    {Lexeme::Type::BRACKET, "BRACKET"},
    {Lexeme::Type::SEPARATOR, "SEPARATOR"},
    {Lexeme::Type::UNDEFINED, "UNDEFINED"},
};

const std::unordered_map<Lexeme::Category, std::string> CATEGORIES{
    {Lexeme::Category::KEYWORD, "KEYWORD"},
    {Lexeme::Category::SPECIAL_SYMBOL, "SPECIAL_SYMBOL"},
    {Lexeme::Category::IDENTIFIER, "IDENTIFIER"},
    {Lexeme::Category::CONSTANT, "CONSTANT"},
};

int main(int argc, char **argv) {
  LexicalAnalyzer analyzer;

  try {
    auto lexemes = analyzer.Analyse(argv[1]);

    std::cout << "VALUE" << "\t\t\t" << "TYPE" << "\t\t\t" << "CATEGORY"
              << "\n";
    for (auto lexeme : lexemes) {
      std::cout << lexeme.value << "\t\t\t" << TYPES.at(lexeme.type)
                << (TYPES.at(lexeme.type).length() < 8    ? "\t\t\t"
                    : TYPES.at(lexeme.type).length() > 11 ? "\t"
                                                          : "\t\t")
                << CATEGORIES.at(lexeme.category) << "\n";
    }
  } catch (std::string exception) {
    std::cerr << exception << std::endl;
  }
}
