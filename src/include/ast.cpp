//
// Created by admin on 2019/9/24.
//

#include "ast.h"

AST *AST::optimize() {
    this->left = this->left == nullptr ? nullptr : this->left->optimize();
    this->right = this->right == nullptr ? nullptr : this->right->optimize();

    if (this->type == AST::OR) {
        optimize_OR();
    }

    return this;
}

void AST::optimize_OR() {
    if (this->left->type == AST::CHARSET && this->right->type == CHARSET) {
        this->charset.insert(this->left->charset.begin(), this->left->charset.end());
        this->charset.insert(this->right->charset.begin(), this->right->charset.end());
        this->type = AST::CHARSET;

        delete this->left;
        delete this->right;

        left = nullptr;
        right = nullptr;
    }
}
