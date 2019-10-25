//
// Created by admin on 2019/10/25.
//

#include "catch.hpp"

#define private public

#include "../src/vm/program.h"
#include <cstring>

TEST_CASE("ast to program") {
    SECTION("charset") {
        auto *charset = new REx::AST(REx::AST::CHARSET);
        charset->add_character('a');

        auto program = (new REx::Program())->compile_charset(charset);
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

        auto program = (new REx::Program())->compile_charset(charset);
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

        auto program = (new REx::Program())->compile_charset(charset);
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