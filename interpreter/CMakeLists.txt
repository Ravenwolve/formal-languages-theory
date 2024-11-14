cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(interpreter)

set(SRC
  src/main.cpp

  src/interpreter.cpp
  
  ../lexical_analyzer/src/analyzer.cpp
  ../semantic_analyzer/src/parser.cpp
)

set(HEADER
  src/interpreter.h
  ../semantic_analyzer/src/entry.h
  ../lexical_analyzer/src/state.h
  ../lexical_analyzer/src/lexeme.h
  ../lexical_analyzer/src/analyzer.h
  ../lexical_analyzer/src/analyzer.h
  ../semantic_analyzer/src/parser.h
)

add_executable(${PROJECT_NAME} ${SRC} ${HEADER})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
