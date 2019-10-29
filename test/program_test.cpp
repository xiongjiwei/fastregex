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

        CHECK(ret->length == 20);
        CHECK(ret->type == REx::AST::AND);
        CHECK(ret->left->length == 10);
        CHECK(ret->left->type == REx::AST::PLUS);
        CHECK(ret->right->length == 10);
        CHECK(ret->right->type == REx::AST::STAR);
    }
}