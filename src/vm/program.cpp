//
// Created by admin on 2019/10/25.
//

#include <cstring>
#include "program.h"

REx::BYTE *REx::Program::to_program_tree(REx::AST *ast) {

    return nullptr;
}

REx::Pro_Tree *REx::Program::compile_charset(AST *ast) {
    auto charset = ast->get_charset();
    auto leaf = new Pro_Tree(ast->type);

    if (charset.none()) { //not possible
        return nullptr;
    }

    if (charset.count() == 1) {
        leaf->init_bytecode(2);
        leaf->set_bytecode_content(0, REx::INSTRUCTIONS::character);
        for (BYTE i = 0;; ++i) {
            if (charset.test(i)) {
                leaf->set_bytecode_content(1, i);
                break;
            }
        }
    } else {
        leaf->init_bytecode(33);
        leaf->set_bytecode_content(0, REx::INSTRUCTIONS::oneof);
        BYTE *bitset_to_byte = cast_to_byte(charset);
        leaf->set_bytecode_content(1, bitset_to_byte, 32);
        delete[] bitset_to_byte;
    }

    return leaf;
}

REx::Pro_Tree *REx::Program::compile_to_program_tree(REx::AST *ast) {
    if (ast == nullptr) {
        return nullptr;
    }

    if (ast->type == AST::CHARSET) {
        return compile_charset(ast);
    }

    auto left = compile_to_program_tree(ast->left);
    auto right = compile_to_program_tree(ast->right);

    auto root = new Pro_Tree(ast->type);
    root->left = left;
    root->right = right;

    size_t length = get_padding_byte_length(ast->type);

    if (left != nullptr) {
        length += left->get_bytecode_length();
    }

    if (right != nullptr) {
        length += right->get_bytecode_length();
    }

    root->set_bytecode_length(length);
    return root;
}

REx::BYTE *REx::Program::cast_to_bytes(int16_t num) {
    BYTE *byte = new BYTE[2];
    memcpy(byte, &num, 2);
    return byte;
}

int REx::Program::get_padding_byte_length(REx::AST::NODETYPE type) {
    int table[] = {
            5,      //or
            8,      //star
            8,      //plus
            5,      //option
            0,      //and
            0,      //repeat
            0,      //charset
    };
    return table[type];
}
