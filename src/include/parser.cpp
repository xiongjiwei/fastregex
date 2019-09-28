//
// Created by admin on 2019/9/23.
//

#include "parser.h"

AST *Parser::regex() {
    return nullptr;
}

//e1 | e2 | e3 | ...
AST *Parser::exper() {
    auto root = term();

    while (restring.size() > 0 && restring[0] == '|' && root != nullptr) {
        restring.remove_prefix();
        if (restring.size() == 0 || restring[0] == '|') {
            error_code |= bad_alternation;
            delete root;
            return nullptr;
        }
        root = collapse_binary_operator(root, term(), AST::OR);
    }

    return root;
}

//e1*e2+e3?...
AST *Parser::term() {

    if (restring.size() > 0 && (restring[0] == '*' || restring[0] == '+' || restring[0] == '?')) {
        error_code |= bad_quantifier;
        return nullptr;
    }

    AST *root = repeat();
    while (restring.size() > 0 && restring[0] != '|' && root != nullptr) {
        root = collapse_binary_operator(root, repeat(), AST::AND);
    }

    return root;
}

//e1*, e2+, e3?, e4{m,n}
AST *Parser::repeat() {
    AST *root = factor();
    if (restring.size() > 0 && root != nullptr && !error_code) {
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

//e1{m,n}
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
                delete root;
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

//(e1e2e3)
AST *Parser::factor() {
    if (restring.size() > 0 && !error_code) {
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
    if (restring.size() >= 2 && restring[0] == '(') {
        restring.remove_prefix();
        root = exper();
        if (!(restring.size() >= 1 && restring[0] == ')')) {
            error_code |= bad_parenthesis;
            delete root;
            return nullptr;
        }

        if (root == nullptr && !error_code) {
            root = new AST(AST::CHARSET);
        }
        restring.remove_prefix();
    }
    return root;
}

AST *Parser::charset() {
    if (restring.size() >= 2) {
        AST* root = new AST(AST::CHARSET);
        restring.remove_prefix();
        bool is_negative = false;
        if (restring[0] == '^') {
            is_negative = true;
            restring.remove_prefix();
        }
        root->is_charset_negative = is_negative;
        while (restring.size() >= 1 && restring[0] != ']') {
            if (restring.size() >= 4 && restring[1] == '-') {
                if (restring[0] > restring[2]) {
                    error_code |= bad_charrange;
                    delete root;
                    return nullptr;
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
            error_code |= bad_square_bracket;
            delete root;
            return nullptr;
        }
        restring.remove_prefix();
        return root;
    } else {
        error_code |= bad_square_bracket;
        return nullptr;
    }
}

AST *Parser::chars() {
    AST *root = nullptr;
    if (restring.size() > 0 && UNHANDLED_CHAR.find(restring[0]) == UNHANDLED_CHAR.end()) {
        root = new AST(AST::NODETYPE::CHARSET);
        if (restring[0] == '\\') {
            restring.remove_prefix();
            if (restring.size() > 0) {
                root->add_character(restring[0]);
            } else {
                delete root;
                error_code |= bad_escape;
                return nullptr;
            }
        } else if (restring[0] == '.') {
            root->is_charset_negative = true;
        } else {
            root->add_character(restring[0]);
        }

        restring.remove_prefix();
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
        delete left;
        delete right;
        return nullptr;
    }

    auto root = new AST(type);
    root->left = left;
    root->right = right;
    return root;
}
