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

    if (this->type == Nodetype::OR) {
        optimize_OR();
    } else if (this->type == Nodetype::STAR) {
        optimize_STAR();
    } else if (this->type == Nodetype::PLUS) {
        optimize_PLUS();
    } else if (this->type == Nodetype::OPTION) {
        optimize_OPTION();
    } else if (this->type == Nodetype::REPEAT) {
        optimize_REPEAT();
    } else if (this->type == Nodetype::AND) {
        optimize_AND();
    }

    return this;
}

void REx::AST::optimize_OR() {
    if (this->left->type == Nodetype::CHARSET && this->right->type == Nodetype::CHARSET) {
        this->charset = this->left->charset | this->right->charset;
        this->type = Nodetype::CHARSET;

        delete this->left;
        delete this->right;

        left = nullptr;
        right = nullptr;
    }
}

void REx::AST::optimize_STAR() {
    if (this->child->type == Nodetype::STAR || this->child->type == Nodetype::PLUS || this->child->type == Nodetype::OPTION) {
        delete_child();

        this->type = Nodetype::STAR;
    }
}

void REx::AST::optimize_PLUS() {
    if (this->child->type == Nodetype::STAR || this->child->type == Nodetype::OPTION) {
        delete_child();

        this->type = Nodetype::STAR;
    } else if (this->child->type == Nodetype::PLUS) {
        delete_child();

        this->type = Nodetype::PLUS;
    }
}

void REx::AST::optimize_OPTION() {
    if (this->child->type == Nodetype::STAR || this->child->type == Nodetype::PLUS) {
        delete_child();

        this->type = Nodetype::STAR;
    } else if (this->child->type == Nodetype::OPTION) {
        delete_child();

        this->type = Nodetype::OPTION;
    }
}

void REx::AST::optimize_AND() {

}

void REx::AST::optimize_REPEAT() {
    if (this->child->type == Nodetype::STAR) {      //a*{m,n} --> a*
        delete_child();

        this->type = Nodetype::STAR;
        this->low = 0;
        this->high = 0;
    } else if (this->child->type == Nodetype::PLUS) {   //a+{m,n} --> a+
        delete_child();

        this->type = Nodetype::PLUS;
        this->low = 0;
        this->high = 0;
    } else if (this->child->type == Nodetype::OPTION) { //a?{m,n} --> a{0,n}
        delete_child();

        this->type = Nodetype::REPEAT;
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
    if (this->type == Nodetype::REPEAT && this->left->type == Nodetype::REPEAT) {
        return false;
    }

    return (this->left == nullptr || this->left->is_valid()) && (this->right == nullptr || this->right->is_valid());
}
