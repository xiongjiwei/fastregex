cmake_minimum_required(VERSION 3.10)
project(fastregexcpp)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILE main.cpp)
add_executable(fastregexcpp ${SOURCE_FILE})

include_directories(regex_lib)
add_subdirectory(src)

target_link_libraries(fastregexcpp regex_lib)
#add_executable(fastregexcpp main.cpp src/include/Fastre.cpp src/include/Fastre.h src/include/parser.cpp src/include/parser.h src/include/restring.cpp src/include/restring.h src/include/nfa.cpp src/include/nfa.h src/include/ast.cpp src/include/ast.h test/test.cpp)
add_subdirectory(test)