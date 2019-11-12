//
// Created by admin on 2019/9/27.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#define private public
#include "../src/lexer/parser.h"

TEST_CASE("should return the right size of string", "[restring][size]") {
    std::string string = "this is a template re string";
    REx::REstring restring(string);
    SECTION("should get right size and char") {
        CHECK(string.size() == restring.size());
        CHECK('t' == restring[0]);
        CHECK('i' == restring[2]);
        CHECK('i' == restring[5]);
    }

    SECTION("should get the last char when index is greater than restring size") {
        CHECK('g' == restring[50]);
    }

    SECTION("should get right size and char when remove_prefix with default argument") {
        restring.remove_prefix();
        CHECK(string.size() - 1 == restring.size());
        CHECK('h' == restring[0]);
        CHECK('i' == restring[1]);
        CHECK('i' == restring[4]);
    }

    SECTION("should get right size and char when remove_prefix with positive argument") {
        restring.remove_prefix(2);
        CHECK(string.size() - 2 == restring.size());
        CHECK('i' == restring[0]);
        CHECK('i' == restring[3]);
    }

    SECTION("should get right size and char when remove_prefix with negative argument") {
        restring.remove_prefix(3);
        restring.remove_prefix(-1);
        CHECK(string.size() - 2 == restring.size());
        CHECK('i' == restring[0]);
        CHECK('i' == restring[3]);
    }

    SECTION("should return 0 and get last char when remove_prefix with argument greater than restring remainder") {
        restring.remove_prefix(50);
        CHECK(0 == restring.size());
        CHECK('g' == restring[0]);
        CHECK('g' == restring[2]);
        CHECK('g' == restring[5]);
    }
}
