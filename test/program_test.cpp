//
// Created by admin on 2019/10/25.
//

#include "catch.hpp"
#include "../src/lexer/parser.h"
#include <cstring>

#define private public

#include "../src/vm/program.h"

TEST_CASE("ast to program") {
    SECTION("charset") {
        auto *charset = new REx::AST(REx::AST::CHARSET);
        charset->add_character('a');

        auto program = REx::Program::compile_charset(charset);
        const REx::BYTE test_code[] = {0x01, 'a'};

        CHECK(program->length == 2);
        CHECK(memcmp(test_code, program->bytecode, program->length) == 0);
    }

    SECTION("charset") {
        auto *charset = new REx::AST(REx::AST::CHARSET);
        charset->add_character(0x00);
        charset->add_character(0x01);
        charset->add_character(0x02);
        charset->add_character(0x08);
        charset->add_character(0x09);
        charset->add_character(0x0a);

        auto program = REx::Program::compile_charset(charset);
        const REx::BYTE test_code[] = {
                0x04,
                0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        CHECK(program->length == 33);
        CHECK(memcmp(test_code, program->bytecode, program->length) == 0);
    }

    SECTION("charset") {
        auto *charset = new REx::AST(REx::AST::CHARSET);
        charset->set_charset_negative();

        auto program = REx::Program::compile_charset(charset);
        const REx::BYTE test_code[] = {
                0x04,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
        };

        CHECK(program->length == 33);
        CHECK(memcmp(test_code, program->bytecode, program->length) == 0);
    }
}

TEST_CASE("translate to program tree") {
    SECTION("") {
        std::string string = "a*";
        REx::REstring restring(string);
        REx::Parser parser(restring);

        auto ret = REx::Program::compile_to_program_tree(parser.exper());

        CHECK(ret->length == 10);
        CHECK(ret->type == REx::AST::STAR);
        CHECK(ret->child->length == 2);
        CHECK(ret->child->type == REx::AST::CHARSET);
    }

    SECTION("") {
        std::string string = "a+b*";
        REx::REstring restring(string);
        REx::Parser parser(restring);

        auto ret = REx::Program::compile_to_program_tree(parser.exper());

        CHECK(ret->length == 17);
        CHECK(ret->type == REx::AST::AND);
        CHECK(ret->left->length == 7);
        CHECK(ret->left->type == REx::AST::PLUS);
        CHECK(ret->right->length == 10);
        CHECK(ret->right->type == REx::AST::STAR);
    }
}

TEST_CASE("bytecode test") {
    SECTION("") {
        std::string string = "[a-z]*(abc)+(a|bc)?";
        REx::REstring restring(string);
        REx::Parser parser(restring);

        REx::AST *ast = parser.exper();
        auto bytecode = (new REx::Program())->to_bytecode(ast);
        REx::BYTE valid_bytecode[] = {
                0x02, 0x00, 0x00, 0x00, 0x00,       //split L1, L2
                0x04,                               //oneof
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x03, 0x00, 0x00,                   //jmp L1
                0x01, 'a',                          //code a
                0x01, 'b',                          //code b
                0x01, 'c',                          //code c
                0x02, 0x00, 0x00, 0x00, 0x00,       //split L1, L2
                0x02, 0x00, 0x00, 0x00, 0x00,       //split L1, L2
                0x02, 0x00, 0x00, 0x00, 0x00,       //split L1, L2
                0x01, 'a',                          //code a
                0x03, 0x00, 0x00,                   //jmp L1
                0x01, 'b',                          //code b
                0x01, 'c',                          //code c
                0x00                                //match
        };
        int16_t line = 5;
        memcpy(valid_bytecode + 1, &line, 2);
        line = 41;
        memcpy(valid_bytecode + 3, &line, 2);
        line = -38;
        memcpy(valid_bytecode + 39, &line, 2);
        line = -6;
        memcpy(valid_bytecode + 48, &line, 2);
        line = 5;
        memcpy(valid_bytecode + 50, &line, 2);

        line = 5;
        memcpy(valid_bytecode + 53, &line, 2);
        line = 19;
        memcpy(valid_bytecode + 55, &line, 2);
        line = 5;
        memcpy(valid_bytecode + 58, &line, 2);
        line = 10;
        memcpy(valid_bytecode + 60, &line, 2);
        line = 7;
        memcpy(valid_bytecode + 65, &line, 2);

        std::bitset<256> atoz;
        for (int i = 'a'; i <= 'z'; ++i) {
            atoz[i] = true;
        }

        memcpy(valid_bytecode + 6, REx::cast_to_byte(atoz), 32);
        CHECK(memcmp(valid_bytecode, bytecode, 72) == 0);
    }
}