//
// Created by admin on 2019/9/24.
//

#ifndef FASTREGEXCPP_AST_H
#define FASTREGEXCPP_AST_H


#include <unordered_set>

class AST {
public:
    enum NODETYPE {
        OR,
        STAR,
        PLUS,
        OPTION,
        AND,
        CHARSET
    };

    explicit AST(const NODETYPE type_) : type(type_) {}

    ~AST() {
        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
    }

    void add_character(char ch) {
        this->chars.insert(ch);
    }

    AST *left = nullptr;
    AST *right = nullptr;

    bool is_charset_negative = false;
    const NODETYPE type;
    std::unordered_set<char> chars;
};


#endif //FASTREGEXCPP_AST_H
