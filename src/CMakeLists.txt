project(fastregex)

set(HEADFILES lexer/fastre.h lexer/ast.h lexer/nfa.h lexer/parser.h lexer/restring.h)

set(SOURCEFILE lexer/ast.cpp lexer/fastre.cpp lexer/nfa.cpp lexer/parser.cpp lexer/restring.cpp)

add_library(regex_lib STATIC ${SOURCEFILE} ${HEADFILES})