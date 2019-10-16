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
                auto correct_ast = new AST(AST::CHARSET);
                correct_ast->add_character('a');
                correct_ast->add_character('b');
                correct_ast->add_character('c');
                correct_ast->add_character('d');
                CHECK((*correct_ast == *test_ret));
            }
        }
    }

    SECTION("optimise star") {
        WHEN("child is star") {
            string = "(a*)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is repeat") {
            string = "(a{1,10})*";
            auto test_ret = parser.exper()->optimize();
        }
    }

    SECTION("optimise plus") {
        WHEN("child is star") {
            string = "(a*)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to plus") {
                auto correct_ast = new AST(AST::PLUS);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)+";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

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
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+)?";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?)?";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to option") {
                auto correct_ast = new AST(AST::OPTION);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

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
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is plus") {
            string = "(a+){2,5}";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to plus") {
                auto correct_ast = new AST(AST::PLUS);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("child is option") {
            string = "(a?){2,5}";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to 0 to high") {
                auto correct_ast = new AST(AST::REPEAT);
                correct_ast->low = 0;
                correct_ast->high = 5;
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

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
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }

        WHEN("star and repeat") {
            string = "(((a)*){1,10})*";
            auto test_ret = parser.exper()->optimize();
            THEN("optimise to star") {
                auto correct_ast = new AST(AST::STAR);
                correct_ast->child = new AST(AST::CHARSET);
                correct_ast->child->add_character('a');

                CHECK((*test_ret == *correct_ast));
            }
        }
    }
}

