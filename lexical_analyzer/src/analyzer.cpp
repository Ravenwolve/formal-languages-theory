#include "analyzer.h"
#include "lexeme.h"

#include <cctype>
#include <iostream>
#include <sstream>
#include <unordered_set>

LexicalAnalyzer::LexicalAnalyzer() : _state(State::START) {}

std::vector<Lexeme> LexicalAnalyzer::Analyse(const std::string &text) {
  std::vector<Lexeme> lexemes;

  const std::unordered_set<char> EXACTLY_SINGLE_SYMBOL_OPERATOR{
      '>', '+', '-', '*', '/', '(', ')', ';'};

  size_t row_number = 1;
  size_t col_number = 1;

  std::string error_message;
  std::stringstream lexeme;
  for (char ch : text) {
    if (ch == '\n') {
      ++row_number;
      col_number = 1;
    }

    switch (_state) {

    case State::START:
      lexeme.str("");

      if (std::isdigit(ch)) {
        _state = State::READING_CONSTANT;

        lexeme << ch;
      } else if (std::isalpha(ch)) {
        _state = State::READING_IDENTIFIER;

        lexeme << ch;
      } else if (EXACTLY_SINGLE_SYMBOL_OPERATOR.contains(ch)) {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at(std::string(1, ch)));
      } else if (ch == '=') {
        _state = State::READING_OPERATOR_ASSIGNMENT_OR_EQUAL;
      } else if (ch == '<') {
        _state = State::READING_OPERATOR_LESS_OR_NOT_EQUAL;
      } else if (std::isspace(ch)) {
      } else {
        _state = State::ERROR;

        error_message = "undefined symbol";
      }
      break;

    case State::READING_IDENTIFIER:
      if (std::isalnum(ch)) {
        lexeme << ch;
      } else if (std::isspace(ch)) {
        _state = State::START;

        std::string ready_made_lexeme = lexeme.str();
        lexemes.push_back(TERMINAL_LEXEMES.contains(ready_made_lexeme)
                              ? TERMINAL_LEXEMES.at(ready_made_lexeme)
                              : Lexeme{Lexeme::Type::UNDEFINED,
                                       Lexeme::Category::IDENTIFIER,
                                       lexeme.str()});
      } else if (EXACTLY_SINGLE_SYMBOL_OPERATOR.contains(ch)) {
        _state = State::START;

        std::string ready_made_lexeme = lexeme.str();
        lexemes.push_back(TERMINAL_LEXEMES.contains(ready_made_lexeme)
                              ? TERMINAL_LEXEMES.at(ready_made_lexeme)
                              : Lexeme{Lexeme::Type::UNDEFINED,
                                       Lexeme::Category::IDENTIFIER,
                                       lexeme.str()});
        lexemes.push_back(TERMINAL_LEXEMES.at(std::string(1, ch)));
      } else if (ch == '=') {
        _state = State::READING_OPERATOR_ASSIGNMENT_OR_EQUAL;

        std::string ready_made_lexeme = lexeme.str();
        lexemes.push_back(TERMINAL_LEXEMES.contains(ready_made_lexeme)
                              ? TERMINAL_LEXEMES.at(ready_made_lexeme)
                              : Lexeme{Lexeme::Type::UNDEFINED,
                                       Lexeme::Category::IDENTIFIER,
                                       lexeme.str()});
      } else if (ch == '<') {
        _state = State::READING_OPERATOR_LESS_OR_NOT_EQUAL;

        std::string ready_made_lexeme = lexeme.str();
        lexemes.push_back(TERMINAL_LEXEMES.contains(ready_made_lexeme)
                              ? TERMINAL_LEXEMES.at(ready_made_lexeme)
                              : Lexeme{Lexeme::Type::UNDEFINED,
                                       Lexeme::Category::IDENTIFIER,
                                       lexeme.str()});
      } else {
        _state = State::ERROR;

        error_message = "incorrect identifier symbol";
      }
      break;

    case State::READING_CONSTANT:
      if (std::isdigit(ch)) {
        lexeme << ch;
      } else if (std::isspace(ch)) {
        _state = State::START;

        lexemes.push_back({Lexeme::Type::UNDEFINED, Lexeme::Category::CONSTANT,
                           lexeme.str()});

      } else if (EXACTLY_SINGLE_SYMBOL_OPERATOR.contains(ch)) {
        _state = State::START;

        lexemes.push_back({Lexeme::Type::UNDEFINED, Lexeme::Category::CONSTANT,
                           lexeme.str()});
        lexemes.push_back(TERMINAL_LEXEMES.at(std::string(1, ch)));
      } else if (ch == '<') {
        std::string ready_made_lexeme = lexeme.str();
        _state = State::READING_OPERATOR_LESS_OR_NOT_EQUAL;

        lexemes.push_back({Lexeme::Type::UNDEFINED, Lexeme::Category::CONSTANT,
                           lexeme.str()});
      } else {
        _state = State::ERROR;

        error_message = "incorrect literal symbol";
      }
      break;

    case State::READING_OPERATOR_ASSIGNMENT_OR_EQUAL:
      if (ch == '=') {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("=="));
      } else if (std::isspace(ch)) {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("="));
      } else if (std::isdigit(ch)) {
        _state = State::READING_CONSTANT;

        lexemes.push_back(TERMINAL_LEXEMES.at("="));

        lexeme.str("");
        lexeme << ch;
      } else if (std::isalpha(ch)) {
        _state = State::READING_IDENTIFIER;

        lexemes.push_back(TERMINAL_LEXEMES.at("="));

        lexeme.str("");
        lexeme << ch;
      } else if (ch == '(') {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("="));
        lexemes.push_back(TERMINAL_LEXEMES.at("("));
      } else {
        _state = State::ERROR;

        error_message = "undefined symbol";
      }
      break;

    case State::READING_OPERATOR_LESS_OR_NOT_EQUAL:
      if (ch == '>') {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("<>"));
      } else if (std::isspace(ch)) {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("<"));
      } else if (std::isdigit(ch)) {
        _state = State::READING_CONSTANT;

        lexemes.push_back(TERMINAL_LEXEMES.at("<"));

        lexeme.str("");
        lexeme << ch;
      } else if (std::isalpha(ch)) {
        _state = State::READING_IDENTIFIER;

        lexemes.push_back(TERMINAL_LEXEMES.at("<"));

        lexeme.str("");
        lexeme << ch;
      } else if (ch == '(') {
        _state = State::START;

        lexemes.push_back(TERMINAL_LEXEMES.at("<"));
        lexemes.push_back(TERMINAL_LEXEMES.at("("));
      } else {
        _state = State::ERROR;

        error_message = "undefined symbol";
      }
      break;
    }

    if (_state == State::ERROR) {
      _state = State::START;
      throw std::to_string(row_number) + ":" + std::to_string(col_number) +
          ": lexical error:" + error_message + " after '" +
          (lexemes.end() - 1)->value + "' lexeme" + "\n";
    }

    ++col_number;
  }

  lexemes.push_back(TERMINAL_LEXEMES.contains(lexeme.str())
                        ? TERMINAL_LEXEMES.at(lexeme.str())
                        : Lexeme{Lexeme::Type::UNDEFINED,
                                 _state == State::READING_IDENTIFIER
                                     ? Lexeme::Category::IDENTIFIER
                                     : Lexeme::Category::CONSTANT,
                                 lexeme.str()});

  _state = State::START;
  return lexemes;
}
