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
        REPEAT,
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
        this->charset.insert(ch);
    }

    AST *left = nullptr;
    AST *right = nullptr;

    int low = 0;
    int high = 0;

    bool is_charset_negative = false;
    const NODETYPE type;
    std::unordered_set<char> charset;

    bool operator==(AST &other) {
        return this->charset == other.charset &&
                this->low == other.low && this->high == other.high &&
                this->type == other.type &&
                this->is_charset_negative == other.is_charset_negative &&
               (this->left == other.left ||
                (this->left != nullptr && other.left != nullptr && *(left) == *(other.left))) &&
               (this->right == other.right ||
                (this->right != nullptr && other.right != nullptr && *(this->right) == *(other.right)));
    }

    AST *optimize();

private:
};


#endif //FASTREGEXCPP_AST_H
