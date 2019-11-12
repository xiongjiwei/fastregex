//
// Created by admin on 2019/9/23.
//

#include <climits>
#include "parser.h"
#include "ast.h"

//^e1$
REx::AST *REx::Parser::regex() {
    AST *root = nullptr;
    if (restring.size() > 0 && restring[0] == '^') {

    }

    root = exper();

    return root;
}

//e1 | e2 | e3 | ...
REx::AST *REx::Parser::exper() {
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
REx::AST *REx::Parser::term() {
    AST *root = repeat();
    while (restring.size() > 0 && restring[0] != '|' && restring[0] != ')' && root != nullptr) {
        root = collapse_binary_operator(root, repeat(), AST::AND);
    }

    return root;
}

//e1*, e2+, e3?, e4{m,n}
REx::AST *REx::Parser::repeat() {

    if (restring.size() > 0 && (restring[0] == '*' || restring[0] == '+' || restring[0] == '?')) {
        restring.remove_prefix();
        set_error_code(bad_quantifier);
        return nullptr;
    }

    AST *root = factor();
    if (restring.size() > 0 && root != nullptr && error_code.none()) {
        if (restring[0] == '*') {
            root = collapse_unary_operator(root, AST::STAR);
            restring.remove_prefix();
        } else if (restring[0] == '?') {
            root = collapse_unary_operator(root, AST::OPTION);
            restring.remove_prefix();
        } else if (restring[0] == '+') {
            root = collapse_unary_operator(root, AST::PLUS);
            restring.remove_prefix();
        } else if (restring[0] == '{') {
            root = maybe_repeat(root);
        }
    }
    return root;
}

//e1{m,n}
REx::AST *REx::Parser::maybe_repeat(AST *root) {
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
                father->child = root;
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
            if (low > high || high == 0) {  //not support e{0,0}
                set_error_code(bad_quantifier);
                delete root;
                return nullptr;
            }
            restring.remove_prefix();
            auto father = new AST(AST::REPEAT);
            father->low = low;
            father->high = high;
            father->child = root;
            return father;
        }
        restring.remove_prefix(-removed_count);
    }
    return root;
}

//(e1e2e3)
REx::AST *REx::Parser::factor() {
    if (restring.size() > 0 && error_code.none()) {
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

REx::AST *REx::Parser::group() {
    AST *root = nullptr;
    if (restring.size() >= 2 && restring[0] == '(') {
        restring.remove_prefix();
        root = exper();
        if (restring.size() == 0 || restring[0] != ')') {
            set_error_code(bad_parenthesis);
            delete root;
            return nullptr;
        }

        if (root == nullptr && error_code.none()) {
            root = new AST(AST::CHARSET);
        }
        restring.remove_prefix();
    } else {
        this->set_error_code(bad_parenthesis);
    }
    return root;
}

REx::AST *REx::Parser::charset() {
    AST *root = nullptr;
    if (restring.size() >= 2) {
        root = new AST(AST::CHARSET);
        restring.remove_prefix();
        bool is_negative = false;
        if (restring[0] == '^') {
            is_negative = true;
            restring.remove_prefix();
        }
        std::vector<char> stake; // use -1 indicate charset like \d\w\s. use -2 indicate operator '-'

        while (restring.size() >= 1) {
            if (restring[0] == ']') {
                restring.remove_prefix();
                for (size_t i = 0; i < stake.size(); ++i) {
                    if (stake.size() - i >= 3 && stake[i] != -1 && stake[i + 1] == -2 &&
                        stake[i + 2] != -1) {  //we have a '-' in mid
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
                        if (stake[i] != -1) { // this char is not \d\w\s..
                            root->add_character(
                                    stake[i] == -2 ? '-'
                                                   : stake[i]); //if stake[i] == -2, it means this is a '-' character
                        }
                    }
                }

                if (is_negative) {
                    root->set_charset_negative();
                }
                return root;
            }
            if (restring[0] == '\\') {
                auto code_set = process_escape();
                if (code_set == nullptr) { // bad escape char
                    delete root;
                    set_error_code(bad_escape);
                    return nullptr;
                }
                if (code_set->size() == 1) {
                    stake.push_back((char) *code_set->begin());
                } else {
                    stake.push_back(-1);    //-1 indicate charset like \d\w\s
                    for (char code : *code_set) {
                        root->add_character((char) code);
                    }
                    delete code_set;
                }
            } else if (restring[0] == '-') {
                stake.push_back(-2); //-2 indicate operator '-'
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

REx::AST *REx::Parser::chars() {
    AST *root = nullptr;
    if (restring.size() > 0 && UNHANDLED_CHAR.find(restring[0]) == UNHANDLED_CHAR.end()) {
        root = new AST(AST::NODETYPE::CHARSET);
        if (restring[0] == '\\') {
            auto code_set = process_escape();
            if (code_set != nullptr) {
                for (char code : *code_set) {
                    root->add_character((char) code);
                }
                delete code_set;
            } else {
                delete root;
                set_error_code(bad_escape);
                return nullptr;
            }
        } else if (restring[0] == '.') {
            root->set_charset_negative();
            restring.remove_prefix();
        } else {
            root->add_character(restring[0]);
            restring.remove_prefix();
        }
    }

    return root;
}

REx::AST *REx::Parser::collapse_unary_operator(AST *child, AST::NODETYPE type) {
    if (child == nullptr) {
        return nullptr;
    }

    auto root = new AST(type);
    root->child = child;
    return root;
}

REx::AST *REx::Parser::collapse_binary_operator(AST *left, AST *right, AST::NODETYPE type) {
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

/// the escape character code point
std::unordered_set<char> *REx::Parser::process_escape() {
    restring.remove_prefix(); // remove \

    std::unordered_set<char> *charset = nullptr;
    if (restring.size() > 0) {
        charset = new std::unordered_set<char>;
        if (restring[0] == 'd' || restring[0] == 'D') {
            for (int i = 0; i < 255; ++i) {
                if (('0' <= i && i <= '9') == (restring[0] == 'd')) {
                    charset->insert(i);
                }
            }
            restring.remove_prefix();
        } else if (restring[0] == 'w' || restring[0] == 'W') {
            for (int i = 0; i < 255; ++i) {
                if ((('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z') || ('0' <= i && i <= '9') || (i == '_')) ==
                    (restring[0] == 'w')) {
                    charset->insert(i);
                }
            }
            restring.remove_prefix();
        } else if (restring[0] == 's' || restring[0] == 'S') {
            for (int i = 0; i < 255; ++i) {
                if (((i == ' ') || (i == '\t') || (i == '\n')) == (restring[0] == 's')) {
                    charset->insert(i);
                }
            }
            restring.remove_prefix();
        } else if (restring[0] == 'x') {
            restring.remove_prefix();
            if (restring.size() > 0 && is_HEX_digital(restring[0])) {
                int code = UnHex(restring[0]);  //first hex number
                restring.remove_prefix();
                if (restring.size() > 0 && is_HEX_digital(restring[0])) {
                    code = code * 16 + UnHex(restring[0]); //second hex number
                    restring.remove_prefix();
                }
                charset->insert(code);
            } else {
                charset->insert(0);// \x match code point 0
            }
        } else if (is_OTC_digital(restring[0])) {
            int code = 0;
            while (is_HEX_digital(restring[0]) && code < 256) {
                code = code * 8 + restring[0] - '0';
                restring.remove_prefix();
            }
            charset->insert(code);
        } else {
            charset->insert(restring[0]);
            restring.remove_prefix();
        }
    }

    return charset;
}
