cmake_minimum_required(VERSION 3.10)
project(fastregexcpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(fastregexcpp src/main.cpp src/include/fastre.cpp src/include/fastre.h src/include/parser.cpp src/include/parser.h src/include/restring.cpp src/include/restring.h src/include/nfa.cpp src/include/nfa.h src/include/ast.cpp src/include/ast.h)