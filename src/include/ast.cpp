//
// Created by admin on 2019/9/24.
//

#include "ast.h"

/**
 *  optimise follow the following rules:
 *        a|b|c|d -> [abcd]
 *        (a*)* -> a*, (a+)* -> a*, (a?)* -> a*,
 *        (a*)+ -> a*, (a+)+ -> a+, (a?)+ -> a*
 *        (a*)? -> a*, (a+)? -> a*, (a?)? -> a?
 *        (a*){m,n} -> a*, (a+){m,n} -> a+, (a?){m,n} -> a{0,n}
 *
 * **/
AST *AST::optimize() {
    this->left = this->left == nullptr ? nullptr : this->left->optimize();
    this->right = this->right == nullptr ? nullptr : this->right->optimize();

    if (this->type == AST::OR) {
        optimize_OR();
    } else if (this->type == AST::STAR) {
        optimize_STAR();
    } else if (this->type == AST::PLUS) {
        optimize_PLUS();
    } else if (this->type == AST::OPTION) {
        optimize_OPTION();
    } else if (this->type == AST::REPEAT) {
        optimize_REPEAT();
    } else if (this->type == AST::AND) {
        optimize_AND();
    }

    return this;
}

void AST::optimize_OR() {
    if (this->left->type == AST::CHARSET && this->right->type == CHARSET) {
        this->charset = this->left->charset | this->right->charset;
        this->type = AST::CHARSET;

        delete this->left;
        delete this->right;

        left = nullptr;
        right = nullptr;
    }
}

void AST::optimize_STAR() {
    if (this->left->type == AST::STAR || this->left->type == AST::PLUS || this->left->type == AST::OPTION) {
        delete_child();

        this->type = AST::STAR;
    }
}

void AST::optimize_PLUS() {
    if (this->left->type == AST::STAR || this->left->type == AST::OPTION) {
        delete_child();

        this->type = AST::STAR;
    } else if (this->left->type == AST::PLUS) {
        delete_child();

        this->type = AST::PLUS;
    }
}

void AST::optimize_OPTION() {
    if (this->left->type == AST::STAR || this->left->type == AST::PLUS) {
        delete_child();

        this->type = AST::STAR;
    } else if (this->left->type == AST::OPTION) {
        delete_child();

        this->type = AST::OPTION;
    }
}

void AST::optimize_AND() {

}

void AST::optimize_REPEAT() {
    if (this->left->type == AST::STAR) {
        delete_child();

        this->type = AST::STAR;
        this->low = 0;
        this->high = 0;
    } else if (this->left->type == AST::PLUS) {
        delete_child();

        this->type = AST::PLUS;
        this->low = 0;
        this->high = 0;
    } else if (this->left->type == AST::OPTION) {
        delete_child();

        this->type = AST::REPEAT;
        this->low = 0;
    }
}

void AST::delete_child() {
    auto t = this->left;
    this->left = this->left->left;
    t->left = nullptr;
    delete t;
}
