cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(parser)

set(SRC
  src/main.cpp

  src/parser.cpp
  
  ../lexical_analyzer/src/analyzer.cpp
)

set(HEADER
  src/parser.h

  src/entry.h

  ../lexical_analyzer/src/state.h
  ../lexical_analyzer/src/lexeme.h
  ../lexical_analyzer/src/analyzer.h
)

add_executable(${PROJECT_NAME} ${SRC} ${HEADER})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
