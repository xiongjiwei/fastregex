//
// Created by admin on 2019/9/23.
//

#include "parser.h"

AST *Parser::exper() {
    auto root = term();
    if (root == nullptr) {
        return nullptr;
    }

    while (restring.size() >= 2 && restring[0] == '|') {
        restring.remove_prefix();
        root = collapse_or(root, term());
    }

    return root;
}

AST *Parser::term() {
    AST *root = factor();
    while (restring.size() > 0 && root != nullptr) {
        switch (restring[0]) {
            case '*':
                restring.remove_prefix();
                root = collapse_star(root);
                break;
            case '?':
                root = collapse_option(root);
                restring.remove_prefix();
                break;
            case '+':
                root = collapse_plus(root);
                restring.remove_prefix();
                break;
            default:
                root = collapse_and(root, factor());
                break;
        }
    }

    return root;
}

AST *Parser::factor() {
    if (restring.size() > 0) {
        switch (restring[0]) {
            case '(':
                return group();
            case '[':
                return charset();
            default:
                return chars();
        }
    }
    return nullptr;
}

AST *Parser::group() {
    AST *root = nullptr;
    if (restring.size() >= 2) {
        restring.remove_prefix();
        root = exper();
        if (!(restring.size() >= 1 && restring[0] == ')')) {
            root = nullptr;
        }
    }
    return root;
}

AST *Parser::charset() {
    AST *root = nullptr;
    if (restring.size() >= 2) {
        restring.remove_prefix();
        bool is_negative = false;
        if (restring[0] == '^') {
            is_negative = true;
            restring.remove_prefix();
        }

        while (restring.size() >= 1 && restring[0] != ']') {
            if (root == nullptr) {
                root = new AST(AST::CHARSET);
                root->is_charset_negative = is_negative;
            }

            if (restring.size() >= 4 && restring[1] == '-') {
                if (restring[0] > restring[2]) {
                    delete root;
                    root = nullptr;
                    break;
                }

                for (char ch = restring[0]; ch <= restring[2]; ++ch) {
                    root->add_character(ch);
                }
                restring.remove_prefix(3);
            } else {
                root->add_character(restring[0]);
                restring.remove_prefix();
            }
        }

        if (restring.size() == 0) {
            delete root;
            root = nullptr;
        } else {
            restring.remove_prefix();
        }
    }

    return root;
}

AST *Parser::chars() {
    AST *root = nullptr;

    while (restring.size() > 0 && UNHANDLED_CHAR.find(restring[0]) == UNHANDLED_CHAR.end()) {
        if (root == nullptr) {
            root = new AST(AST::NODETYPE::CHARSET);
        }
        root->add_character(restring[0]);
    }

    return root;
}
