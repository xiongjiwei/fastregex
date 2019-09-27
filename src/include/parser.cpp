//
// Created by admin on 2019/9/23.
//

#include "parser.h"

AST *Parser::regex() {
    return nullptr;
}

AST *Parser::exper() {
    auto root = term();
    if (root == nullptr) {
        return nullptr;
    }

    while (restring.size() >= 2 && restring[0] == '|') {
        restring.remove_prefix();
        root = collapse_binary_operator(root, term(), AST::OR);
    }

    return root;
}

AST *Parser::term() {
    AST *root = repeat();
    while (restring.size() > 0 && restring[0] != '|' && root != nullptr) {
        root = collapse_binary_operator(root, repeat(), AST::AND);
    }

    return root;
}

AST *Parser::repeat() {
    AST *root = factor();
    if (restring.size() > 0 && root != nullptr) {
        if (restring[0] == '*') {
            root = collapse_unary_operator(root, AST::STAR);
        } else if (restring[0] == '?') {
            root = collapse_unary_operator(root, AST::OPTION);
        } else if (restring[0] == '+') {
            root = collapse_unary_operator(root, AST::PLUS);
        } else if (restring[0] == '{') {
            root = maybe_repeat(root);
        }
    }
    return root;
}

AST *Parser::maybe_repeat(AST *root) {
    if (restring.size() > 0 && restring[0] == '{') {
        restring.remove_prefix();
        int removed_count = 0;
        int low = 0;
        int high = -1; //fixme should use max_int instead
        while (restring.size() > 0 && restring[0] <= '9' && restring[0] >= '0') {
            low *= 10;
            low += restring[0] - '0';
            restring.remove_prefix();
            removed_count++;
        }

        if (removed_count == 0) {
            return root;
        }

        if (restring.size() > 0 && restring[0] == ',') {
            restring.remove_prefix();
            removed_count++;
        } else {
            return root;
        }

        while (restring.size() > 0 && restring[0] <= '9' && restring[0] >= '0') {
            high = high == -1 ? 0 : high;
            high += restring[0] - '0';
            restring.remove_prefix();
            removed_count++;
        }

        if (restring.size() > 0 && restring[0] == '}') {
            if (low > high) {
                return nullptr;
            }
            restring.remove_prefix();
            auto father = new AST(AST::REPEAT);
            father->low = low;
            father->high = high;
            father->left = root;
            return father;
        }
        restring.remove_prefix(-removed_count);
    }
    return root;
}

AST *Parser::factor() {
    if (restring.size() > 0) {
        if (restring[0] == '(') {
            return group();
        } else if (restring[0] == '[') {
            return charset();
        } else {
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

AST *Parser::collapse_unary_operator(AST *child, AST::NODETYPE type) {
    if (child == nullptr) {
        return nullptr;
    }

    auto root = new AST(type);
    root->left = child;
    return root;
}

AST *Parser::collapse_binary_operator(AST *left, AST *right, AST::NODETYPE type) {
    if (left == nullptr || right == nullptr) {
        return nullptr;
    }

    auto root = new AST(type);
    root->left = left;
    root->right = right;
    return root;
}
