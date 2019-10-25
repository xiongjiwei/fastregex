//
// Created by admin on 2019/10/25.
//

#include <cstring>
#include "program.h"

REx::BYTE *REx::Program::to_program(REx::AST *ast) {

    return nullptr;
}

void REx::Program::compile_to_bytecode(REx::AST *ast) {
    if (ast->type == AST::CHARSET) {
        compile_charset(ast);
    }
}

REx::Sub_program * REx::Program::compile_charset(AST *ast) {
    auto charset = ast->get_charset();
    auto sub_program = new Sub_program();

    if (charset.none()) { //not possible
        return nullptr;
    }

    if (charset.count() == 1) {
        sub_program->bytecode = new BYTE[2];
        sub_program->bytecode[0] = REx::INSTRUCTIONS::character;
        for (BYTE i = 0;; ++i) {
            if (charset.test(i)) {
                sub_program->bytecode[1] = i;
                sub_program->length = 2;
                break;
            }
        }
    } else {
        sub_program->bytecode = new BYTE[33];
        sub_program->bytecode[0] = REx::INSTRUCTIONS::oneof;
        BYTE *bitset_to_byte = cast_to_byte(charset);
        memcpy(sub_program->bytecode + 1, bitset_to_byte, 32);
        delete [] bitset_to_byte;
        sub_program->length = 33;
    }

    return sub_program;
}
