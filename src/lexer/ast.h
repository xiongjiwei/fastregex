//
// Created by admin on 2019/9/24.
//

#ifndef FASTREGEXCPP_AST_H
#define FASTREGEXCPP_AST_H

#include "../re/rex.h"

namespace REx {

    class AST {
    public:
        enum NODETYPE {
            OR,
            STAR,
            PLUS,
            OPTION,
            AND,
            REPEAT,
            CHARSET
        };

    private:
        friend class Program;
        friend class Parser;
        explicit AST(const NODETYPE type_) : type(type_) {}

        ~AST() {
            delete left;
            delete right;
            left = nullptr;
            right = nullptr;
        }

        NODETYPE type;
        AST *left = nullptr;
        AST *right = nullptr;
        AST*& child = left;

        int low = 0;
        int high = 0;
        std::bitset<256> charset;

        void add_character(char ch) {
            this->charset[ch] = true;
        }

        std::bitset<256> & get_charset() {
            return charset;
        }

        void set_charset_negative() {
            this->charset.flip();
        }

        bool operator==(AST &other) {
            return this->charset == other.charset &&
                   this->low == other.low && this->high == other.high &&
                   this->type == other.type &&
                   (this->left == other.left ||
                    (this->left != nullptr && other.left != nullptr && *(left) == *(other.left))) &&
                   (this->right == other.right ||
                    (this->right != nullptr && other.right != nullptr && *(this->right) == *(other.right)));
        }

        AST *optimize();
        bool is_valid();

        void optimize_OR();
        void optimize_STAR();
        void optimize_PLUS();
        void optimize_OPTION();
        void optimize_AND();
        void optimize_REPEAT();

        void delete_child();
    };
}


#endif //FASTREGEXCPP_AST_H
