//
// Created by admin on 2019/10/25.
//

#ifndef FASTREGEXCPP_PROGRAM_H
#define FASTREGEXCPP_PROGRAM_H

#include <cstring>
#include "../re/rex.h"
#include "../lexer/ast.h"

namespace REx {

    class Pro_Tree {
    public:
        AST::NODETYPE type;
        Pro_Tree *left = nullptr;
        Pro_Tree *&child = left;
        Pro_Tree *right = nullptr;

        Pro_Tree(AST::NODETYPE type) {
            this->type = type;
        };

        ~Pro_Tree() {
            delete left;
            delete right;
            delete[] bytecode;
        };

        void init_bytecode(int16_t _length) {
            bytecode = new BYTE[_length];
            this->length = _length;
        }

        void set_bytecode_length(int16_t _length) {
            this->length = _length;
        }

        void set_bytecode_content(int16_t pos, BYTE byte) {
            bytecode[pos] = byte;
        }

        void set_bytecode_content(int16_t pos, BYTE *byte, int16_t n) {
            memcpy(bytecode + pos, byte, n);
        }

        size_t get_bytecode_length() const{
            return this->length;
        }

    private:
        size_t length;
        BYTE *bytecode;
    };

    class Program {
    public:
        BYTE *to_program_tree(AST *ast);
    private:
        static Pro_Tree * compile_charset(AST *ast);
        static Pro_Tree * compile_to_program_tree(AST *ast);

        static int get_padding_byte_length(AST::NODETYPE type);
        static BYTE * cast_to_bytes(int16_t num);
        BYTE *program;
    };
}


#endif //FASTREGEXCPP_PROGRAM_H
