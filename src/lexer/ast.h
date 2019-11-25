//
// Created by admin on 2019/9/24.
//

#ifndef FASTREGEXCPP_AST_H
#define FASTREGEXCPP_AST_H

#include "../re/rex.h"

namespace REx {
    class Data {
    public:
        friend class AST;
        union {
            std::bitset<256> charset = 0;
            struct {
                int low;
                int high;
            };
        };

        void add_character(char ch) {
            this->charset[ch] = true;
        }

        void set_charset_negative() {
            this->charset.flip();
        }

        bool operator==(Data &other) {
            return charset == other.charset && low == other.low && high == other.high;
        }

    private:
        Data(){};
        ~Data(){};
    };

    class AST {
    private:
        friend class Program;
        friend class Parser;
        friend class Fastre;
        explicit AST(const Nodetype type_) : type(type_) {
            if (type_ == Nodetype::REPEAT || type_ == Nodetype::CHARSET) {
                value = new Data();
            }
        }

        ~AST() {
            delete left;
            delete right;
            delete value;
            left = nullptr;
            right = nullptr;
            value = nullptr;
        }

        Nodetype type;
        AST *left = nullptr;
        AST *right = nullptr;
        AST*& child = left;
        Data *value = nullptr;

        bool operator==(AST &other) {
            return (this->value == other.value ||
                    (this->value != nullptr && other.value != nullptr && *(this->value) == *(other.value))) &&
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
