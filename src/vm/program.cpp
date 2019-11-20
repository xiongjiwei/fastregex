//
// Created by admin on 2019/10/25.
//

#include <cstring>
#include <climits>
#include "program.h"
REx::BYTE * REx::Program::program = nullptr;

REx::BYTE *REx::Program::to_bytecode(AST *ast) {
    auto pro_tree = compile_to_program_tree(ast);
    if (pro_tree == nullptr) {
        return nullptr;
    }
    delete ast;
    init_program(pro_tree);
    marshal_program(0, pro_tree);
    REx::Program::program[pro_tree->length] = 0x00;

    delete pro_tree;
    return REx::Program::program;
}

void REx::Program::init_program(const Pro_Tree *pro_tree) {
    REx::Program::program = new BYTE[pro_tree->length + 1];
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

    root->length = get_padding_byte_length(ast);
    root->low = ast->low;
    root->high = ast->high;

    if (left != nullptr) {
        root->length += left->length;
        if (root->low != 0 && root->low != root->high) {
            root->length += left->length;
        }
    }

    if (right != nullptr) {
        root->length += right->length;
    }

    return root;
}

void REx::Program::marshal_int16(int16_t pos, int16_t num) {
    memcpy(program + pos, &num, 2);
}

void REx::Program::marshal_instruction(int16_t pos, REx::BYTE instruction) {
    program[pos] = instruction;
}

void REx::Program::marshal_content(int16_t pos, REx::BYTE *bytecode, int16_t length) {
    memcpy(program + pos, bytecode, length);
}

int REx::Program::get_padding_byte_length(AST *ast) {
    int padding = 0;
    int table[] = {
            8,      //or
            8,      //star
            5,      //plus
            5,      //option
            0,      //and
            0,      //repeat
            0,      //charset
    };

    if (ast->type == AST::REPEAT) {
        if (ast->low != 0) {
            padding += 6;
        }

        if (ast->high == INT_MAX) {
            padding += 8;
        } else if (ast->high != ast->low) {
            padding += 11;
        }
    }

    return table[ast->type] + padding;
}


void REx::Program::marshal_program(int16_t pos, REx::Pro_Tree *pro_tree) {
    switch (pro_tree->type) {
        case AST::OR:
            marshal_or(pos, pro_tree);
            break;
        case AST::STAR:
            marshal_star(pos, pro_tree);
            break;
        case AST::PLUS:
            marshal_plus(pos, pro_tree);
            break;
        case AST::OPTION:
            marshal_option(pos, pro_tree);
            break;
        case AST::AND:
            marshal_and(pos, pro_tree);
            break;
        case AST::REPEAT:
            marshal_repeat(pos, pro_tree);
            break;
        case AST::CHARSET:
            marshal_charset(pos, pro_tree);
            break;
    }
}

/*
 * ------------------------------------------------------------------------
 * |L0                   |L1         |            |L2         |L3         |
 * | split |  L1  |  L2  |···········| jmp |  L3  |···········|···········|
 * ------------------------------------------------------------------------
 * |   1   |   2  |   2  |     n     |  1  |   2  |     n     |···········|
 * ------------------------------------------------------------------------
 * */
void REx::Program::marshal_or(int16_t pos, Pro_Tree *pro_tree) {
    marshal_instruction(pos, INSTRUCTIONS::split);
    marshal_int16(pos + 1, 1 + 2 + 2);
    marshal_int16(pos + 1 + 2, 1 + 2 + 2 + pro_tree->left->length + 1 + 2);

    marshal_program(pos + 1 + 2 + 2, pro_tree->left);

    marshal_instruction(pos + 1 + 2 + 2 + pro_tree->left->length, INSTRUCTIONS::jmp);
    marshal_int16(pos + 1 + 2 + 2 + pro_tree->left->length + 1, 1 + 2 + pro_tree->right->length);

    marshal_program(pos + 1 + 2 + 2 + pro_tree->left->length + 1 + 2, pro_tree->right);
}

/*
 * ---------------------------------------------------------
 * |L0                   |L1                        |L2    |
 * | split |  L1  |  L2  |·············| jmp |  L0  |······|
 * ---------------------------------------------------------
 * |   1   |   2  |   2  |      n      |  1  |   2  |······|
 * ---------------------------------------------------------
 * */
void REx::Program::marshal_star(int16_t pos, Pro_Tree *pro_tree) {
    marshal_instruction(pos, INSTRUCTIONS::split);
    marshal_int16(pos + 1, 1 + 2 + 2);
    marshal_int16(pos + 1 + 2, 1 + 2 + 2 + pro_tree->child->length + 1 + 2);

    marshal_program(pos + 1 + 2 + 2, pro_tree->child);
    marshal_instruction(pos + 1 + 2 + 2 + pro_tree->child->length, INSTRUCTIONS::jmp);
    marshal_int16(pos + 1 + 2 + 2 + pro_tree->child->length + 1, -(1 + 2 + 2 + pro_tree->child->length));
}

/*
 * --------------------------------------------
 * |L0                                 |L1    |
 * |·············| split |  L0  |  L1  |······|
 * --------------------------------------------
 * |      n      |   1   |   2  |   2  |······|
 * --------------------------------------------
 */
void REx::Program::marshal_plus(int16_t pos, Pro_Tree *pro_tree) {
    marshal_program(pos, pro_tree->child);

    marshal_instruction(pos + pro_tree->child->length, INSTRUCTIONS::split);
    marshal_int16(pos + pro_tree->child->length + 1, -pro_tree->child->length);
    marshal_int16(pos + pro_tree->child->length + 1 + 2, 1 + 2 + 2);
}


/*
 * --------------------------------------------
 * |L0                   |L1           |L2    |
 * | split |  L1  |  L2  |·············|······|
 * --------------------------------------------
 * |   1   |   2  |   2  |      n      |······|
 * --------------------------------------------
 * */
void REx::Program::marshal_option(int16_t pos, Pro_Tree *pro_tree) {
    marshal_instruction(pos, INSTRUCTIONS::split);
    marshal_int16(pos + 1, 1 + 2 + 2);
    marshal_int16(pos + 1 + 2, 1 + 2 + 2 + pro_tree->child->length);

    marshal_program(pos + 1 + 2 + 2, pro_tree->child);
}


/*
 * -----------------------------
 * |L0           |L1           |
 * |·············|·············|
 * -----------------------------
 * |      n      |      n      |
 * -----------------------------
 */
void REx::Program::marshal_and(int16_t pos, Pro_Tree *pro_tree) {
    marshal_program(pos, pro_tree->left);
    marshal_program(pos + pro_tree->left->length, pro_tree->right);
}


/*
 * -----------------------------------------------------------------------------------------------------------
 * |              |L0                      |L1                              |L2                      |L3     |
 * | loop | times |·········| endloop | L0 | loop | times | split | L2 | L3 |·········| endloop | L1 | other |
 * -----------------------------------------------------------------------------------------------------------
 * |  1   |   2   |    n    |    1    |  2 |   1  |   2   |   1   |  2 |  2 |    n    |    1    |  2 | other |
 * -----------------------------------------------------------------------------------------------------------
 */
void REx::Program::marshal_repeat(int16_t pos, REx::Pro_Tree *pro_tree) {
    if (pro_tree->low != 0) {
        marshal_instruction(pos, INSTRUCTIONS::loop);
        marshal_int16(pos + 1, pro_tree->low);

        marshal_program(pos + 1 + 2, pro_tree->child);

        marshal_instruction(pos + 1 + 2 + pro_tree->child->length, INSTRUCTIONS::endloop);
        marshal_int16(pos + 1 + 2 + pro_tree->child->length + 1, -(pro_tree->child->length));

        pos += (1 + 2 + pro_tree->child->length + 1 + 2);
    }

    if (pro_tree->high == INT_MAX) {
        marshal_instruction(pos, INSTRUCTIONS::split);
        marshal_int16(pos + 1, 1 + 2 + 2);
        marshal_int16(pos + 1 + 2, 1 + 2 + 2 + pro_tree->child->length + 1 + 2);

        marshal_program(pos + 1 + 2 + 2, pro_tree->child);
        marshal_instruction(pos + 1 + 2 + 2 + pro_tree->child->length, INSTRUCTIONS::jmp);
        marshal_int16(pos + 1 + 2 + 2 + pro_tree->child->length + 1, -(1 + 2 + 2 + pro_tree->child->length));
    } else if (pro_tree->high != pro_tree->low) {
        marshal_instruction(pos, INSTRUCTIONS::loop);
        marshal_int16(pos + 1, pro_tree->high - pro_tree->low);
        marshal_instruction(pos + 1 + 2, INSTRUCTIONS::split);
        marshal_int16(pos + 1 + 2 + 1, 1 + 2 + 2);
        marshal_int16(pos + 1 + 2 + 1 + 2, 1 + 2 + 2 + pro_tree->child->length + 1 + 2);

        marshal_program(pos + 1 + 2 + 1 + 2 + 2, pro_tree->child);

        marshal_instruction(pos + 1 + 2 + 1 + 2 + 2 + pro_tree->child->length, INSTRUCTIONS::endloop);
        marshal_int16(pos + 1 + 2 + 1 + 2 + 2 + pro_tree->child->length + 1, -(1 + 2 + 2 + pro_tree->child->length));
    }
}

void REx::Program::marshal_charset(int16_t pos, Pro_Tree *pro_tree) {
    marshal_content(pos, pro_tree->bytecode, pro_tree->length);
}