cmake_minimum_required(VERSION 3.10)
project(fastregexcpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(fastregexcpp main.cpp include/fastre.cpp include/fastre.h include/parser.cpp include/parser.h include/restring.cpp include/restring.h include/nfa.cpp include/nfa.h include/ast.cpp include/ast.h)