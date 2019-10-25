//
// Created by admin on 2019/10/25.
//

#ifndef FASTREGEXCPP_PROGRAM_H
#define FASTREGEXCPP_PROGRAM_H

#include "../re/rex.h"
#include "../lexer/ast.h"

namespace REx {

    typedef struct Sub_p{
        size_t length;
        BYTE *bytecode;

        Sub_p(){}
    } Sub_program;

    class Program {
    public:
        BYTE *to_program(AST *ast);
        void compile_to_bytecode(AST *ast);
    private:
        Sub_program * compile_charset(AST *ast);
        BYTE *program;
    };
}


#endif //FASTREGEXCPP_PROGRAM_H
