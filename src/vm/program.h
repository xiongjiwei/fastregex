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
        explicit Pro_Tree(Nodetype type) {
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

        void set_bytecode_content(int16_t pos, BYTE byte) {
            bytecode[pos] = byte;
        }

        void set_bytecode_content(int16_t pos, BYTE *byte, int16_t n) {
            memcpy(bytecode + pos, byte, n);
        }
        size_t length = 0;
        BYTE *bytecode = nullptr;
        Nodetype type;
        Pro_Tree *left = nullptr;
        Pro_Tree *&child = left;
        Pro_Tree *right = nullptr;

        int low = 0;
        int high = 0;
    };
    class Program {
    private:
        friend class Fastre;
        static BYTE *to_bytecode(AST *ast);
        static Pro_Tree * compile_charset(AST *ast);
        static Pro_Tree * compile_to_program_tree(AST *ast);
        static void marshal_program(int16_t pos, REx::Pro_Tree *pro_tree);
        static void marshal_or(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_star(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_plus(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_option(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_and(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_repeat(int16_t pos, Pro_Tree *pro_tree);
        static void marshal_charset(int16_t pos, Pro_Tree *pro_tree);

        static void init_program(const Pro_Tree *pro_tree);
        static int get_padding_byte_length(AST *ast);
        static void marshal_int16(int16_t pos, int16_t num);
        static void marshal_instruction(int16_t pos, REx::BYTE instruction);
        static void marshal_content(int16_t pos, BYTE *bytecode, int16_t length);
        static BYTE *program;
    };
}


#endif //FASTREGEXCPP_PROGRAM_H
