//
// Created by admin on 2019/9/27.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/include/restring.h"

TEST_CASE("should return the size of string", "[string size]") {
    std::string string = "a template re string";
    REstring restring(string);

    CHECK(string.size() == restring.size());
}

TEST_CASE("should return the correct char", "[char]") {
    size_t a = 10;
    size_t b = 20;

    size_t c = a - b;

    CHECK(-10 == c);
}
