//
// Created by admin on 2019/9/23.
//

#include "parser.h"

bool Parser::regex() {
    if (restring.size() > 0) {
        if (restring[0] == '^') {
            restring.remove_prefix(1);
            exper();
            return restring.size() == 1 && restring[0] == '$';
        } else {
            exper();
            return true;
        }
    } else {
        return false;
    }
}

void Parser::exper() {
    term();
    while (restring[0] == '|') {
        restring.remove_prefix(1);
        term();
    }
}

void Parser::term() {
    while (true) {
        factor();
        switch (restring[0]) {
            case '+':
                return;
            case '*':
                return;
            case '?':
                return;
            default:
                return;
        }
    }
}

void Parser::factor() {
    switch (restring[0]) {
        case '[':
            charset();
            break;
        case '(':
            group();
            break;
        default:
            chars();
    }
}

void Parser::charset() {
    if (restring[0] == '^') {
        restring.remove_prefix(1);
        range();
    } else {
        range();
    }
}

void Parser::group() {
    exper();
}

void Parser::chars() {
    if (restring[0] == '\\') {
        restring.remove_prefix(1);

    }
}

void Parser::range() {

}

void Parser::push_operator(Operation::operations op, size_t parameter_count) {
    if (nfa_nodes.size() >= parameter_count) {
        //todo: handle ops!
    } else {
        ops.push(new Operation(op, parameter_count));
    }
}

void Parser::push_nfa_node(NFA *push_node) {
    //fixme: ops.top()->get_parameters_count() will not less than nfa_nodes.size() + 1
    if (ops.top()->get_parameters_count() <= nfa_nodes.size() + 1) {
        switch (ops.top()->get_op()) {
            case Operation::OPTION:
                break;
            case Operation::OR:
                break;
            case Operation::STAR:
                break;
            case Operation::PLUS:
                break;
            default:
                break;
        }
    }
}

