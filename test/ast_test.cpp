//
// Created by admin on 2019/10/11.
//

#include "catch.hpp"
#include "../src/include/ast.h"

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

