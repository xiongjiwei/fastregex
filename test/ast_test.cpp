//
// Created by admin on 2019/10/11.
//

#include "catch.hpp"
#define private public
#include "../src/lexer/ast.h"
#include "../src/lexer/parser.h"

TEST_CASE("operator ==") {
    SECTION("should return true when pointer is same") {
        auto left = new REx::AST(REx::Nodetype::STAR);
        auto right = left;

        CHECK((*left == *right));
    }

    SECTION("should return true when content is same") {
        auto left = new REx::AST(REx::Nodetype::STAR);
        left->left = new REx::AST(REx::Nodetype::CHARSET);
        left->left->value->add_character('a');

        auto right = new REx::AST(REx::Nodetype::STAR);
        right->left = new REx::AST(REx::Nodetype::CHARSET);
        right->left->value->add_character('a');

        CHECK((*left == *right));
    }

    SECTION("should return false when anyone is not same") {
        auto left = new REx::AST(REx::Nodetype::STAR);
        left->left = new REx::AST(REx::Nodetype::CHARSET);
        left->left->value->add_character('a');

        auto right = new REx::AST(REx::Nodetype::STAR);

        CHECK_FALSE((*left == *right));
    }
}

TEST_CASE("optimize") {
    std::string string;
    REx::REstring restring(string);
    REx::Parser parser(restring);
    SECTION("nothing to optimise") {
        string = "a*";
        auto test_ret = parser.exper()->optimize();
        restring.remove_prefix(-2);
        auto correct_ast = parser.exper();

        CHECK((*correct_ast == *test_ret));
    }

    SECTION("optimise or") {
        WHEN("left and right are all charset type") {
            string = "a|b|c|d";
            auto test_ret = parser.exper()->optimize();

            THEN("optimise to charset") {
                auto correct_ast = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->value->add_character('a');
                correct_ast->value->add_character('b');
                correct_ast->value->add_character('c');
                correct_ast->value->add_character('d');
                CHECK((*correct_ast == *test_ret));
            }
        }
    }

    SECTION("optimise star") {
        WHEN("child is star") {
            string = "(a*)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }
    }

    SECTION("optimise plus") {
        WHEN("child is star") {
            string = "(a*)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to plus") {
                auto correct_ast = new REx::AST(REx::Nodetype::PLUS);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is repeat") {
            string = "(a{1,10})+";
            auto test_ret = parser.exper()->optimize();

        }
    }

    SECTION("optimise option") {
        WHEN("child is star") {
            string = "(a*)?";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)?";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)?";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to option") {
                auto correct_ast = new REx::AST(REx::Nodetype::OPTION);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is repeat") {
            string = "(a{1,10})?";
            auto test_ret = parser.exper()->optimize();
        }
    }

    SECTION("optimise repeat") {
        WHEN("child is star") {
            string = "(a*){2,5}";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+){2,5}";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to plus") {
                auto correct_ast = new REx::AST(REx::Nodetype::PLUS);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?){2,5}";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to 0 to high") {
                auto correct_ast = new REx::AST(REx::Nodetype::REPEAT);
                correct_ast->value->low = 0;
                correct_ast->value->high = 5;
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is repeat") {
            string = "(a{1,10}){2,5}";
            auto test_ret = parser.exper()->optimize();
        }
    }

    SECTION("nested") {
        WHEN("star") {
            string = "(((a)*)*)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("star and repeat") {
            string = "(((a)*){1,10})*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new REx::AST(REx::Nodetype::STAR);
                correct_ast->child = new REx::AST(REx::Nodetype::CHARSET);
                correct_ast->child->value->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }
    }
}

