project(fastregex)

set(HEADFILES re/fastre.h lexer/ast.h lexer/parser.h)

set(SOURCEFILE lexer/ast.cpp re/fastre.cpp lexer/parser.cpp vm/vm.cpp vm/vm.h re/rex.h vm/program.cpp vm/program.h lexer/test.cpp lexer/test.h)

add_library(regex_lib STATIC ${SOURCEFILE} ${HEADFILES})