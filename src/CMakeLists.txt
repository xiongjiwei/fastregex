project(fastregex)

set(HEADFILES re/fastre.h lexer/ast.h lexer/nfa.h lexer/parser.h)

set(SOURCEFILE lexer/ast.cpp re/fastre.cpp lexer/nfa.cpp lexer/parser.cpp vm/ReVM.cpp vm/ReVM.h re/rex.h)

add_library(regex_lib STATIC ${SOURCEFILE} ${HEADFILES})