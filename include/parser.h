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
    AST* range();
    AST* group();
    AST* chars();

    bool push_to_ast_stake(AST *);

    void push_operator(Operation::operations op, size_t parameter_count);
    void push_nfa_node(nfa_graph*);

private:
    REstring& restring;
    const std::unordered_set<char> UNHANDLED_CHAR = {'*', '+', '?', ')', '|',};

    AST * collapse_star(AST *child);
    AST * collapse_plus(AST *child);
    AST * collapse_option(AST *child);
    AST * collapse_or(AST *left, AST *right);
    AST * collapse_and(AST *left, AST *right);
    void collapse_char(char ch);
};


#endif //FASTREGEXCPP_PARSER_H
