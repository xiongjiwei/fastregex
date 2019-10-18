project(fastregex)

set(HEADFILES lexer/fastre.h lexer/ast.h lexer/nfa.h lexer/parser.h)

set(SOURCEFILE lexer/ast.cpp lexer/fastre.cpp lexer/nfa.cpp lexer/parser.cpp vm/ReVM.cpp vm/ReVM.h)

add_library(regex_lib STATIC ${SOURCEFILE} ${HEADFILES})