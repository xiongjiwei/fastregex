//
// Created by jwxiong on 2019/11/14.
//

#include "catch.hpp"
#define private public
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

        WHEN("") {
            string = "\\d*[123]";
            fastre.compile(string);

            CHECK(fastre.full_match("553"));
            CHECK(fastre.full_match("55223"));
            CHECK(fastre.full_match("55123"));
            CHECK_FALSE(fastre.full_match("5"));
        }

        WHEN("") {
            string = "\\d{4,6}";
            fastre.compile(string);

            CHECK(fastre.full_match("5533"));
            CHECK_FALSE(fastre.full_match("512"));
            CHECK_FALSE(fastre.full_match("512s"));
        }

        WHEN("") {
            string = ".*";
            fastre.compile(string);

            CHECK(fastre.full_match("5533"));
            CHECK(fastre.full_match("512"));
            CHECK(fastre.full_match("512s"));
        }

        WHEN("") {
            string = ".{12,90}";
            fastre.compile(string);

            CHECK_FALSE(fastre.full_match("512oidoixlcvlkglduoiuzsiouc"));  //lasy, only match 12 chars
            CHECK_FALSE(fastre.full_match("5533"));
        }
    }


    SECTION("wrong regex") {
        std::string string;
        REx::Fastre fastre;
        WHEN("bad escape") {
            string = "\\";
            fastre.compile(string);

            CHECK(fastre.bytecode == nullptr);
            CHECK(fastre.error_index == 1);
            CHECK(fastre.error_msg == "bad escape");
        }

        WHEN("bad square bracket") {
            string = "abcg[1-3";
            fastre.compile(string);

            CHECK(fastre.bytecode == nullptr);
            CHECK(fastre.error_index == 8);
            CHECK(fastre.error_msg == "bad square bracket");
        }
    }
}