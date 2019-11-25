//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_PARSER_H
#define FASTREGEXCPP_PARSER_H


#include <unordered_set>
#include <vector>
#include <string>
#include "ast.h"
namespace REx {
    class REstring {
    private:
        friend class Parser;
        friend class Fastre;

        explicit REstring(const std::string &pattern_) : pattern(pattern_) {}

        void remove_prefix(int count = 1) {
            cur_index += count;
        }

        size_t size() const {
            return pattern.size() >= cur_index ? pattern.size() - cur_index : 0;
        }

        char operator[](const size_t index_) const {
            return index_ + cur_index >= pattern.size() ? pattern[pattern.size() - 1] : pattern[index_ + cur_index];
        }
        const std::string &pattern;
        size_t cur_index = 0;
    };

    class Parser {
    private:
        friend class Fastre;
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
        std::unordered_set<char> * process_escape();

        AST * collapse_unary_operator(AST *child, Nodetype type);
        AST * collapse_binary_operator(AST *left, AST* right, Nodetype type);

        static const unsigned char bad_escape =         1;
        static const unsigned char bad_parenthesis =    2;
        static const unsigned char bad_quantifier =     3;
        static const unsigned char bad_charrange =      4;
        static const unsigned char bad_square_bracket = 5;
        static const unsigned char bad_alternation =    6;

        bool get_error_code(unsigned char code) const {
            return error_code[code];
        }

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
            error_code[code] = true;
            error_index = restring.cur_index;
            error_msg = error_msgs[code];
        }
        REstring& restring;
        const std::unordered_set<char> UNHANDLED_CHAR = {'*', '+', '?', ')', '|',};

        static const std::string error_msgs[8];
        std::bitset<8> error_code;
        int error_index = 0;
        std::string error_msg;
    };
}


#endif //FASTREGEXCPP_PARSER_H
