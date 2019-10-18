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
REx::AST *REx::AST::optimize() {
    this->left = this->left == nullptr ? nullptr : this->left->optimize();
    this->right = this->right == nullptr ? nullptr : this->right->optimize();

    if (this->type == AST::OR) {
        optimize_OR();
    } else if (this->type == REx::AST::STAR) {
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

void REx::AST::optimize_OR() {
    if (this->left->type == AST::CHARSET && this->right->type == CHARSET) {
        this->charset = this->left->charset | this->right->charset;
        this->type = AST::CHARSET;

        delete this->left;
        delete this->right;

        left = nullptr;
        right = nullptr;
    }
}

void REx::AST::optimize_STAR() {
    if (this->child->type == REx::AST::STAR || this->child->type == AST::PLUS || this->child->type == AST::OPTION) {
        delete_child();

        this->type = AST::STAR;
    }
}

void REx::AST::optimize_PLUS() {
    if (this->child->type == REx::AST::STAR || this->child->type == AST::OPTION) {
        delete_child();

        this->type = AST::STAR;
    } else if (this->child->type == REx::AST::PLUS) {
        delete_child();

        this->type = AST::PLUS;
    }
}

void REx::AST::optimize_OPTION() {
    if (this->child->type == REx::AST::STAR || this->child->type == REx::AST::PLUS) {
        delete_child();

        this->type = AST::STAR;
    } else if (this->child->type == AST::OPTION) {
        delete_child();

        this->type = REx::AST::OPTION;
    }
}

void REx::AST::optimize_AND() {

}

void REx::AST::optimize_REPEAT() {
    if (this->child->type == REx::AST::STAR) {
        delete_child();

        this->type = REx::AST::STAR;
        this->low = 0;
        this->high = 0;
    } else if (this->child->type == REx::AST::PLUS) {
        delete_child();

        this->type = REx::AST::PLUS;
        this->low = 0;
        this->high = 0;
    } else if (this->child->type == REx::AST::OPTION) {
        delete_child();

        this->type = REx::AST::REPEAT;
        this->low = 0;
    }
}

void REx::AST::delete_child() {
    auto t = this->child;
    this->child = this->child->child;
    t->child = nullptr;
    delete t;
}

bool REx::AST::is_valid() {
    //not support expressions like ((((a{1,100})){1,100}){1,100}){1,100}
    if (this->type == REPEAT && this->left->type == REPEAT) {
        return false;
    }

    return (this->left == nullptr || this->left->is_valid()) && (this->right == nullptr || this->right->is_valid());
}
