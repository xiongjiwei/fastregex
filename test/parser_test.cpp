//
// Created by admin on 2019/9/26.
//

#include "catch.hpp"
#include "../src/include/parser.h"

TEST_CASE("chars() method should build correct AST by given regular expression") {
    std::string string = R"(f\)\4\x\x10.\)";
    REstring restring(string);
    Parser parser(restring);
    std::vector<AST *> results;
    SECTION("match the pattern") {
        WHEN("restring moved forward") {
            auto test_ast = parser.chars();
            AND_WHEN("chars() method meet a normal char") {
                std::unordered_set<char> test_charset = {'f'};

                THEN("build a normal char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a escape character") {
                std::unordered_set<char> test_charset = {')'};

                THEN("build a normal char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a escape otc character") {
                std::unordered_set<char> test_charset = {04};

                THEN("build an otc code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a escape hex character without code point") {
                std::unordered_set<char> test_charset = {0x0};

                THEN("build a 0 hex code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a escape hex character with code point") {
                std::unordered_set<char> test_charset = {0x10};

                THEN("build a hex code point AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK_FALSE(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a dot") {
                std::unordered_set<char> test_charset = {};

                THEN("build an any char AST") {
                    CHECK(test_ast->charset == test_charset);
                    CHECK(test_ast->is_charset_negative);
                    results.push_back(test_ast);
                }
            }

            test_ast = parser.chars();
            AND_WHEN("chars() method meet a bad escape") {
                THEN("get an error") {
                    CHECK(test_ast == nullptr);
                    CHECK(parser.get_error_code(Parser::bad_escape));
                }
            }
        }

        WHEN("check other attribute") {
            for (auto item: results) {
                CHECK(item->high == 0);
                CHECK(item->low == 0);
                CHECK(item->left == nullptr);
                CHECK(item->right == nullptr);
                CHECK(item->type == AST::CHARSET);
                delete item;
            }
        }
    }
}

TEST_CASE("charset() method should build correct AST by given regular expression") {
    GIVEN("charset regex") {
        std::string string = "[abc]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {'a', 'b', 'c'};
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK_FALSE(test_ret->is_charset_negative);
            }
        }
    }

    GIVEN("charset regex with range") {
        std::string string = "[a-zA-Z]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {};
            for (char i = 'a'; i <= 'z'; ++i) {
                test_charset.insert(i);
            }
            for (char i = 'A'; i <= 'Z'; ++i) {
                test_charset.insert(i);
            }
            THEN("should return a charset type AST with a-z and A-Z") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK_FALSE(test_ret->is_charset_negative);
            }
        }
    }

    GIVEN("charset regex with wrong range") {
        std::string string = "[z-a]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            THEN("should return null and set error code bad_charrange") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_charrange));
            }
        }
    }

    GIVEN("charset regex with char '-'") {
        std::string string = "[a-zA-]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {};
            for (char i = 'a'; i <= 'z'; ++i) {
                test_charset.insert(i);
            }
            test_charset.insert('A');
            test_charset.insert('-');
            THEN("should return a charset type AST with a-z and A and -") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK_FALSE(test_ret->is_charset_negative);
            }
        }
    }

    GIVEN("charset regex with escape") {
        std::string string = R"([\a\x-\x4)\-])";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {};
            for (char i = 0; i <= 4; ++i) {
                test_charset.insert(i);
            }
            test_charset.insert('a');
            test_charset.insert('-');
            test_charset.insert(')');
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK_FALSE(test_ret->is_charset_negative);
            }
        }
    }

    GIVEN("charset regex with negative") {
        std::string string = "[^a-c]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {};
            for (char i = 'a'; i <= 'c'; ++i) {
                test_charset.insert(i);
            }
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->is_charset_negative);
            }
        }
    }

    GIVEN("charset regex with bad square bracket") {
        std::string string = "[a-cA-Z";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret == nullptr);
                CHECK((parser.get_error_code(Parser::bad_square_bracket)));
            }
        }
    }

    GIVEN("charset regex with bad escape") {
        std::string string = "[a-cA-Z\\]";
        REstring restring(string);
        Parser parser(restring);

        WHEN("building") {
            auto test_ret = parser.charset();
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret == nullptr);
                CHECK((parser.get_error_code(Parser::bad_square_bracket)));
            }
        }
    }
}