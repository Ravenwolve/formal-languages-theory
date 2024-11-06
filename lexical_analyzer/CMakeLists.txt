cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(lexer)

set(SRC
  src/main.cpp

  src/analyzer.cpp
)

set(HEADER
  src/state.h
  
  src/lexeme.h
  src/analyzer.h
)

add_executable(${PROJECT_NAME} ${SRC} ${HEADER})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
