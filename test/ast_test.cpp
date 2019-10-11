//
// Created by admin on 2019/10/11.
//

#include "catch.hpp"
#include "../src/include/ast.h"
#include "../src/include/parser.h"

TEST_CASE("operator ==") {
    SECTION("should return true when pointer is same") {
        auto left = new AST(AST::STAR);
        left->add_character('a');
        left->low = 1;
        left->high = 2;

        auto right = left;

        CHECK((*left == *right));
    }

    SECTION("should return true when content is same") {
        auto left = new AST(AST::STAR);
        left->add_character('a');
        left->low = 1;
        left->high = 2;
        left->left = new AST(AST::CHARSET);

        auto right = new AST(AST::STAR);
        right->add_character('a');
        right->low = 1;
        right->high = 2;
        right->left = new AST(AST::CHARSET);

        CHECK((*left == *right));
    }

    SECTION("should return false when anyone is not same") {
        auto left = new AST(AST::STAR);
        left->add_character('a');
        left->low = 1;
        left->high = 2;
        left->left = new AST(AST::CHARSET);

        auto right = new AST(AST::STAR);
        right->add_character('a');
        right->low = 1;
        right->high = 2;

        CHECK_FALSE((*left == *right));
    }
}

TEST_CASE("optimize") {
    std::string string;
    REstring restring(string);
    Parser parser(restring);
    WHEN("nothing to optimise") {
        string = "a*";
        auto test_ret = parser.exper()->optimize();
        restring.remove_prefix(-2);
        auto correct_ast = parser.exper();


        THEN("should same as no optimise version") {
            CHECK((*correct_ast == *test_ret));
        }
    }

    WHEN("optimise or") {
        string = "a|b|c|d";
        auto test_ret = parser.exper()->optimize();
        auto correct_ast = new AST(AST::CHARSET);
        correct_ast->add_character('a');
        correct_ast->add_character('b');
        correct_ast->add_character('c');
        correct_ast->add_character('d');

        THEN("should be a charset type") {
            CHECK((*correct_ast == *test_ret));
        }
    }
}

