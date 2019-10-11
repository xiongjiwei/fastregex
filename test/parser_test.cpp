//
// Created by admin on 2019/9/26.
//

#include "catch.hpp"
#include <climits>
#include <iostream>
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
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("regex is normal charset") {
            string = "[abc]";
            auto test_ret = parser.charset();
            std::unordered_set<char> test_charset = {'a', 'b', 'c'};
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret->charset == test_charset);
                CHECK(test_ret->type == AST::CHARSET);
                CHECK_FALSE(test_ret->is_charset_negative);
            }
        }

        WHEN("regex is range") {
            string = "[a-zA-Z]";
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

        WHEN("regex is illegal range") {
            string = "[z-a]";
            auto test_ret = parser.charset();
            THEN("should return null and set error code bad_charrange") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_charrange));
            }
        }

        WHEN("regex with a '-'") {
            string = "[a-zA-]";
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

        WHEN("regex with escape") {
            string = R"([\a\x-\x4)\-])";
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


        WHEN("regex with negative") {
            string = "[^a-c]";
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

        WHEN("regex with bad square bracket") {
            string = "[a-cA-Z";
            auto test_ret = parser.charset();
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret == nullptr);
                CHECK((parser.get_error_code(Parser::bad_square_bracket)));
            }
        }


        WHEN("regex with bad escape") {
            string = "[a-cA-Z\\]";
            auto test_ret = parser.charset();
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret == nullptr);
                CHECK((parser.get_error_code(Parser::bad_square_bracket)));
            }
        }
    }
}

TEST_CASE("group() method should build correct AST by given regular expression") {
    GIVEN("group regex") {
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("regex with char") {
            string = "(a)";
            auto test_ret = parser.group();
            THEN("should return a charset type AST with a") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->charset == std::unordered_set<char> {'a'});
            }
        }

        WHEN("regex with nothing") {
            string = "()";
            auto test_ret = parser.group();
            THEN("should return a empty AST") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->charset.empty());
            }
        }
    }
}

TEST_CASE("factor() method should build correct AST by given regular expression") {
    GIVEN("factor regex") {
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("regex with normal char") {
            string = "a";
            auto test_ret = parser.factor();
            THEN("should return a charset type AST with a") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->charset == std::unordered_set<char>{'a'});
            }
        }


        WHEN("regex with charset") {
            string = "[abc]";
            auto test_ret = parser.factor();
            THEN("should return a charset type AST with abc") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->charset == std::unordered_set<char>{'a', 'b', 'c'});
            }
        }

        WHEN("regex with group") {
            string = "(a)";
            auto test_ret = parser.factor();
            THEN("should return a charset type AST with a") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(test_ret->charset == std::unordered_set<char>{'a'});
            }
        }


        WHEN("illegal factor regex") {
            string = "(";
            auto test_ret = parser.factor();
            THEN("should return null") {
                CHECK(test_ret == nullptr);
            }
        }
    }
}

TEST_CASE("repeat() method should build correct AST by given regular expression") {
    GIVEN("repeat regex") {
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("regex with star") {
            string = "a*";
            auto test_ret = parser.repeat();
            THEN("should return a star type AST") {
                CHECK(test_ret->type == AST::STAR);
                CHECK(test_ret->left->type == AST::CHARSET);
                CHECK(0 == restring.size());
            }
        }

        WHEN("regex with {m}") {
            string = "b{2}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::REPEAT);
                CHECK(test_ret->low == 2);
                CHECK(test_ret->high == 2);
                CHECK(test_ret->left->type == AST::CHARSET);
            }
        }

        WHEN("regex with {m,}") {
            string = "b{2,}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::REPEAT);
                CHECK(test_ret->low == 2);
                CHECK(test_ret->high == INT_MAX);
                CHECK(test_ret->left->type == AST::CHARSET);
            }
        }

        WHEN("regex with {m,n}") {
            string = "b{2,5}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::REPEAT);
                CHECK(test_ret->low == 2);
                CHECK(test_ret->high == 5);
                CHECK(test_ret->left->type == AST::CHARSET);
            }
        }

        WHEN("illegal range") {
            string = "b{20,5}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_quantifier));
            }
        }

        WHEN("no left") {
            string = "b{,5}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(restring.size() == 4);
            }
        }

        WHEN("not repeat") {
            string = "b{1,5";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(restring.size() == 4);
            }
        }

        WHEN("not repeat") {
            string = "b{-,5}";
            auto test_ret = parser.repeat();
            THEN("should return a repeat type AST") {
                CHECK(test_ret->type == AST::CHARSET);
                CHECK(restring.size() == 5);
            }
        }
    }
}

TEST_CASE("term() method should build correct AST by given regular expression") {
    GIVEN("term regex") {
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("regex normal") {
            string = "ab*c";
            auto test_ret = parser.term();
            AST *correct_ast = new AST(AST::AND);
            correct_ast->left = new AST(AST::AND);
            correct_ast->left->left = new AST(AST::CHARSET);
            correct_ast->left->left->add_character('a');
            correct_ast->left->right = new AST(AST::STAR);
            correct_ast->left->right->left = new AST(AST::CHARSET);
            correct_ast->left->right->left->add_character('b');
            correct_ast->right = new AST(AST::CHARSET);
            correct_ast->right->add_character('c');
            THEN("should return a AST") {
                CHECK((*test_ret == *correct_ast));
                CHECK(restring.size() == 0);
            }
        }

        WHEN("regex with '|'") {
            string = "ab*c|";
            auto test_ret = parser.term();
            AST *correct_ast = new AST(AST::AND);
            correct_ast->left = new AST(AST::AND);
            correct_ast->left->left = new AST(AST::CHARSET);
            correct_ast->left->left->add_character('a');
            correct_ast->left->right = new AST(AST::STAR);
            correct_ast->left->right->left = new AST(AST::CHARSET);
            correct_ast->left->right->left->add_character('b');
            correct_ast->right = new AST(AST::CHARSET);
            correct_ast->right->add_character('c');
            THEN("should return a AST") {
                CHECK((*test_ret == *correct_ast));
                CHECK(restring.size() == 1);
            }
        }

        WHEN("regex with bad quantifier") {
            string = "*a";
            auto test_ret = parser.term();
            THEN("should return a AST") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_quantifier));
            }
        }
    }
}

TEST_CASE("exper() method should build correct AST by given regular expression") {
    GIVEN("regex") {
        std::string string;
        REstring restring(string);
        Parser parser(restring);

        WHEN("normal") {
            string = "a(a|b*|c+d?){1,2}}[a-z]*\\x}]";
            auto test_ret = parser.exper();
            AST *correct_ast = new AST(AST::AND);
            AST *repeat12 = new AST(AST::REPEAT);

            repeat12->low = 1;
            repeat12->high = 2;
            repeat12->left = new AST(AST::OR);
            repeat12->left->left = new AST(AST::OR);
            repeat12->left->left->left = new AST(AST::CHARSET);
            repeat12->left->left->left->add_character('a');
            repeat12->left->left->right = new AST(AST::STAR);
            repeat12->left->left->right->left = new AST(AST::CHARSET);
            repeat12->left->left->right->left->add_character('b');
            repeat12->left->right = new AST(AST::AND);
            repeat12->left->right->left = new AST(AST::PLUS);
            repeat12->left->right->left->left = new AST(AST::CHARSET);
            repeat12->left->right->left->left->add_character('c');
            repeat12->left->right->right = new AST(AST::OPTION);
            repeat12->left->right->right->left = new AST(AST::CHARSET);
            repeat12->left->right->right->left->add_character('d');

            AST *andatoz = new AST(AST::AND);
            andatoz->left = new AST(AST::AND);
            andatoz->left->left = new AST(AST::AND);
            andatoz->left->right = new AST(AST::CHARSET);
            andatoz->left->right->add_character('}');
            andatoz->left->left->left = new AST(AST::CHARSET);
            andatoz->left->left->left->add_character('a');
            andatoz->left->left->right = repeat12;
            andatoz->right = new AST(AST::STAR);
            andatoz->right->left = new AST(AST::CHARSET);
            for (char i = 'a'; i <= 'z'; ++i) {
                andatoz->right->left->add_character(i);
            }

            correct_ast->left = new AST(AST::AND);
            correct_ast->left->left = new AST(AST::AND);
            correct_ast->left->left->left = andatoz;
            correct_ast->left->left->right = new AST(AST::CHARSET);
            correct_ast->left->left->right->add_character(0);
            correct_ast->left->right = new AST(AST::CHARSET);
            correct_ast->left->right->add_character('}');
            correct_ast->right = new AST(AST::CHARSET);
            correct_ast->right->add_character(']');

            auto r = (*test_ret == *correct_ast);
            THEN("should return a AST") {
                CHECK((*test_ret == *correct_ast));
                CHECK(restring.size() == 0);
            }
        }

        WHEN("normal") {
            string = "(a*)*";
            auto test_ret = parser.exper();
            AST *correct_ast = new AST(AST::STAR);
            correct_ast->left = new AST(AST::STAR);
            correct_ast->left->left = new AST(AST::CHARSET);
            correct_ast->left->left->add_character('a');

            THEN("should return a AST with double star") {
                CHECK((*test_ret == *correct_ast));
                CHECK(restring.size() == 0);
            }
        }

        WHEN("nested parenthesis") {
            string = "((ab)(a|b)*)*";
            auto test_ret = parser.exper();
            AST *correct_ast = new AST(AST::STAR);
            correct_ast->left = new AST(AST::AND);
            correct_ast->left->left = new AST(AST::AND);
            correct_ast->left->left->left = new AST(AST::CHARSET);
            correct_ast->left->left->left->add_character('a');
            correct_ast->left->left->right = new AST(AST::CHARSET);
            correct_ast->left->left->right->add_character('b');
            correct_ast->left->right = new AST(AST::STAR);
            correct_ast->left->right->left = new AST(AST::OR);
            correct_ast->left->right->left->left = new AST(AST::CHARSET);
            correct_ast->left->right->left->left->add_character('a');
            correct_ast->left->right->left->right = new AST(AST::CHARSET);
            correct_ast->left->right->left->right->add_character('b');

            THEN("should return a AST with double star") {
                CHECK((*test_ret == *correct_ast));
                CHECK(restring.size() == 0);
            }
        }

        WHEN("bad parenthesis") {
            string = "(a*";
            auto test_ret = parser.exper();

            THEN("should return null with bad_parenthesis error code") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_parenthesis));
            }
        }

        WHEN("bad charragne") {
            string = "[z-a]";
            auto test_ret = parser.exper();

            THEN("should return null with bad_charrange error code") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_charrange));
            }
        }

        WHEN("bad alternation") {
            string = "(a|b|c)|c|";
            auto test_ret = parser.exper();

            THEN("should return null with bad_alternation error code") {
                CHECK(test_ret == nullptr);
                CHECK(parser.get_error_code(Parser::bad_alternation));
            }
        }
    }
}