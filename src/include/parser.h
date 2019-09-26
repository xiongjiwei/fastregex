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
    AST* factor();
    AST* charset();
    AST* group();
    AST* chars();

    void maybe_repeat();
private:
    REstring& restring;
    const std::unordered_set<char> UNHANDLED_CHAR = {'*', '+', '?', ')', '|',};

    static AST * collapse_unary_operator(AST *child, AST::NODETYPE type);
    static AST * collapse_binary_operator(AST *left, AST* right, AST::NODETYPE type);
};


#endif //FASTREGEXCPP_PARSER_H
