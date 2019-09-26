project(fastregex)

set(HEADFILES include/fastre.h include/ast.h include/nfa.h include/parser.h include/restring.h)

set(SOURCEFILE include/ast.cpp include/fastre.cpp include/nfa.cpp include/parser.cpp include/restring.cpp)

add_library(regex_lib STATIC ${SOURCEFILE} ${HEADFILES})