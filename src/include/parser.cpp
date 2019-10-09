//
// Created by admin on 2019/9/23.
//

#include <climits>
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
            set_error_code(bad_alternation);
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
        set_error_code(bad_quantifier);
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
        int removed_count = 1;
        int low = 0;

        int high = INT_MAX;
        while (restring.size() > 0 && restring[0] <= '9' && restring[0] >= '0') {
            low *= 10;
            low += restring[0] - '0';
            restring.remove_prefix();
            removed_count++;
        }

        if (removed_count == 1) {
            restring.remove_prefix(-removed_count);
            return root;
        }

        if (restring.size() > 0 && (restring[0] == ',' || restring[0] == '}')) {
            if (restring[0] == ',') {
                restring.remove_prefix();
                removed_count++;
            } else {
                high = low;
                restring.remove_prefix();
                auto father = new AST(AST::REPEAT);
                father->low = low;
                father->high = high;
                father->left = root;
                return father;
            }
        } else {
            restring.remove_prefix(-removed_count);
            return root;
        }

        while (restring.size() > 0 && restring[0] <= '9' && restring[0] >= '0') {
            high = high == INT_MAX ? 0 : high;
            high += restring[0] - '0';
            restring.remove_prefix();
            removed_count++;
        }

        if (restring.size() > 0 && restring[0] == '}') {
            if (low > high) {
                set_error_code(bad_quantifier);
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
            set_error_code(bad_parenthesis);
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
    AST *root = nullptr;
    if (restring.size() >= 2) {
        root = new AST(AST::CHARSET);
        restring.remove_prefix();
        bool is_negative = false;
        if (restring[0] == '^') {
            is_negative = true;
            restring.remove_prefix();
        }
        root->is_charset_negative = is_negative;
        std::vector<char> stake; // use -2 instead operator '-'

        while (restring.size() >= 1) {
            if (restring[0] == ']') {
                restring.remove_prefix();
                for (int i = 0; i < int(stake.size()); ++i) {
                    if (stake.size() - i >= 3 && stake[i + 1] == -2) {  //we have a '-' in mid
                        if (stake[i] <= stake[i + 2]) {// left char point should no-greater than right char point
                            for (int j = stake[i]; j <= stake[i + 2]; ++j) {
                                root->add_character(char(j));
                            }
                            i += 2; //move i forward 3 step
                        } else { //bad char range
                            set_error_code(bad_charrange);
                            delete root;
                            return nullptr;
                        }
                    } else { //we do not have a '-' in mid
                        root->add_character(
                                stake[i] == -2 ? '-' : stake[i]); //if stake[i] == -2, it means this is a '-' character
                    }
                }
                return root;
            }
            if (restring[0] == '\\') {
                int s = process_escape();
                if (s == -1) { // bad escape char
                    delete root;
                    set_error_code(bad_escape);
                    return nullptr;
                }
                stake.push_back(char(s));
            } else if (restring[0] == '-') {
                stake.push_back(-2); // use -2 instead operator '-'
                restring.remove_prefix();
            } else {
                stake.push_back(restring[0]);
                restring.remove_prefix();
            }
        }
    }

    set_error_code(bad_square_bracket);
    delete root;
    return nullptr;
}

AST *Parser::chars() {
    AST *root = nullptr;
    if (restring.size() > 0 && UNHANDLED_CHAR.find(restring[0]) == UNHANDLED_CHAR.end()) {
        root = new AST(AST::NODETYPE::CHARSET);
        if (restring[0] == '\\') {
            int char_code = process_escape();
            if (char_code != -1) {
                root->add_character((char) char_code);
            } else {
                delete root;
                set_error_code(bad_escape);
                return nullptr;
            }
        } else if (restring[0] == '.') {
            root->is_charset_negative = true;
            restring.remove_prefix();
        } else {
            root->add_character(restring[0]);
            restring.remove_prefix();
        }
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

/// \return the escape character code point, -1 means wrong
int Parser::process_escape() {
    restring.remove_prefix(); // remove \

    if (restring.size() > 0) {
        if (restring[0] == 'x') {
            restring.remove_prefix();
            if (restring.size() > 0 && is_HEX_digital(restring[0])) {
                int code = UnHex(restring[0]);  //first hex number
                restring.remove_prefix();
                if (restring.size() > 0 && is_HEX_digital(restring[0])) {
                    code = code * 16 + UnHex(restring[0]); //second hex number
                    restring.remove_prefix();
                }
                return code;
            } else {
                return 0; // \x match code point 0
            }
        } else {
            if (is_OTC_digital(restring[0])) {
                int code = 0;
                while (is_HEX_digital(restring[0]) && code < 256) {
                    code = code * 8 + restring[0] - '0';
                    restring.remove_prefix();
                }
                return code;
            } else {
                char t = restring[0];
                restring.remove_prefix();
                return t;
            }
        }
    } else {
        return -1;
    }
}
