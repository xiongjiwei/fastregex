//
// Created by admin on 2019/9/26.
//

#include "catch.hpp"
#include "../src/include/parser.h"

TEST_CASE("chars() method should build correct AST by give regular expression") {
    REstring restring(R"(f)\)\4\x\x10.)");
    Parser parser(restring);
    std::vector<AST *> results;
    SECTION("match the pattern") {
        WHEN("restring moved forward") {
            AND_WHEN("chars() method meet a normal char") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {'f'};

                THEN("build a normal char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            AND_WHEN("chars() method meet a wrong parenthesis") {
                auto test_ast = parser.chars();

                THEN("get an error") {
                    CHECK(test_ast == nullptr);
                    CHECK(parser.get_error_code(Parser::bad_parenthesis));
                }
            }

            AND_WHEN("chars() method meet a escape character") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {')'};

                THEN("build a normal char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            AND_WHEN("chars() method meet a escape otc character") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {04};

                THEN("build an otc code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            AND_WHEN("chars() method meet a escape hex character without code point") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {0x0};

                THEN("build a 0 hex code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            AND_WHEN("chars() method meet a escape hex character with code point") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {0x10};

                THEN("build a hex code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            AND_WHEN("chars() method meet a dot") {
                auto test_ast = parser.chars();
                std::unordered_set<char> test_charset = {};

                THEN("build an any char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }
        }

        SECTION("check other attribute") {
            for (auto item: results) {
                CHECK(item->high == 0);
                CHECK(item->low == 0);
                CHECK(item->left == nullptr);
                CHECK(item->right == nullptr);
                CHECK(item->type == AST::CHARSET);
            }
        }
    }
}