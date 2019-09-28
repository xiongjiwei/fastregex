//
// Created by admin on 2019/9/26.
//

#include "catch.hpp"
#include "../src/include/parser.h"

TEST_CASE("should build right AST by given regular expression") {
//    REstring restrings[10] = {"this is a template re string"};
    std::string patterns[] = {
            "this is a template re string",
            "g(a|b|cd)",
            "(a|b|c)*",
            "(a|b|c)+",
            "(a|b|c)?",
            "()?",
            "a[7-9a-z\\x10-\\x1f]",
            "a{1,9}"
    };

    REstring restring("this is a template re string");
    Parser parser(restring);


}