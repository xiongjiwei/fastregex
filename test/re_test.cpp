//
// Created by jwxiong on 2019/11/14.
//

#include "catch.hpp"
#include "../src/re/fastre.h"

TEST_CASE("match test") {
    SECTION("any") {
        std::string string;
        REx::Fastre fastre;
        WHEN("") {
            string = "[a-z]*(abc)+(a|bc)?";
            fastre.compile(string);

            CHECK(fastre.full_match("abcdabcabcbc"));
            CHECK_FALSE(fastre.full_match("A"));
        }

        SECTION("") {
            string = "\\d*[123]";
            fastre.compile(string);

            CHECK(fastre.full_match("553"));
            CHECK(fastre.full_match("55223"));
            CHECK(fastre.full_match("55123"));
            CHECK_FALSE(fastre.full_match("5"));
        }

        SECTION("") {
            string = "\\d{4,6}";
            fastre.compile(string);

            CHECK(fastre.full_match("5533"));
            CHECK_FALSE(fastre.full_match("512"));
            CHECK_FALSE(fastre.full_match("512s"));
        }
    }

}