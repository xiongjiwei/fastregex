//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_PARSER_H
#define FASTREGEXCPP_PARSER_H


#include <unordered_set>
#include "restring.h"
#include "nfa.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(REstring& restring_): restring(restring_){}

    AST* regex();
    AST* exper();
    AST* term();
    AST* repeat();
    AST* factor();
    AST* charset();
    AST* group();
    AST* chars();
    AST* sub_char();

    AST *maybe_repeat(AST *root);

private:
    REstring& restring;
    const std::unordered_set<char> UNHANDLED_CHAR = {'*', '+', '?', ')', '|',};

    static AST * collapse_unary_operator(AST *child, AST::NODETYPE type);
    static AST * collapse_binary_operator(AST *left, AST* right, AST::NODETYPE type);

    unsigned char error_code = 0;
//    const unsigned char base = 1;
    const unsigned char bad_escape = 1 << 1;
    const unsigned char bad_parenthesis = 1 << 2;
    const unsigned char bad_quantifier = 1 << 3;
    const unsigned char bad_charrange = 1 << 4;
    const unsigned char bad_square_bracket = 1 << 5;
};


#endif //FASTREGEXCPP_PARSER_H
