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

    AST *maybe_repeat(AST *root);

private:
    REstring& restring;
    const std::unordered_set<char> UNHANDLED_CHAR = {'*', '+', '?', ')', '|',};

    static AST * collapse_unary_operator(AST *child, AST::NODETYPE type);
    static AST * collapse_binary_operator(AST *left, AST* right, AST::NODETYPE type);

    int process_escape();
    static bool is_HEX_digital(const char c) {
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    }

    static bool is_OTC_digital(const char c) {
        return '0' <= c && c <= '7';
    }

    static int UnHex(char c) {
        if ('0' <= c && c <= '9')
            return c - '0';
        if ('A' <= c && c <= 'F')
            return c - 'A' + 10;
        if ('a' <= c && c <= 'f')
            return c - 'a' + 10;
        return 0;
    }

    void set_error_code(unsigned char code) {
        error_code |= code;
    }

    unsigned char error_code = 0;
//    const unsigned char base = 1;
    const unsigned char bad_escape = 1 << 1;
    const unsigned char bad_parenthesis = 1 << 2;
    const unsigned char bad_quantifier = 1 << 3;
    const unsigned char bad_charrange = 1 << 4;
    const unsigned char bad_square_bracket = 1 << 5;
    const unsigned char bad_alternation = 1 << 6;
};


#endif //FASTREGEXCPP_PARSER_H
