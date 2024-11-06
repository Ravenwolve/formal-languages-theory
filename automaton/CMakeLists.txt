cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(automaton)

find_package(nlohmann_json 3.11.0 REQUIRED)

set(SRC
  src/main.cpp

  src/state.cpp
  src/movestable.cpp
  src/automaton.cpp

  src/dfa.cpp
  src/nfa.cpp
  src/epsnfa.cpp
  
  src/out.cpp
  src/log.cpp
  src/functional.cpp
)

set(HEADER
  src/state.h
  src/movestable.h
  src/automaton.h

  src/dfa.h
  src/nfa.h
  src/epsnfa.h
  
  src/out.h
  src/log.h
  src/functional.h
)

add_executable(${PROJECT_NAME} ${SRC} ${HEADER})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
