//
// Created by admin on 2019/9/23.
//

#include "parser.h"

bool Parser::regex() {
    if (restring.size() > 0) {
        if (restring[0] == '^') {
            restring.remove_prefix();
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
        restring.remove_prefix();
        term();
        collapse_or();
    }
}

void Parser::term() {
    while (true) {
        factor();
        switch (restring[0]) {
            case '+':
                collapse_plus();
                break;
            case '*':
                collapse_star();
                break;
            case '?':
                collapse_option();
                break;
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
        restring.remove_prefix();
        range();
    } else {
        range();
    }
}

bool Parser::group() {
    if (restring[0] == '(' && restring.size() >= 2) {
        restring.remove_prefix();
        exper();
        if (restring.size() >= 1 && restring[0] == ')') {
            restring.remove_prefix();
            return true;
        }
    }
    return false;
}

bool Parser::chars() {
    while (restring.size() > 0 &&
           UNHANDLED_CHAR.find(restring[0]) == UNHANDLED_CHAR.end()) { //not can handled character
        if (restring[0] == '\\') {
            if (restring.size() > 1) {
                //escape character
            } else {
                return false;
            }
        }

        collapse_char(restring[0]);
        restring.remove_prefix();
    }

    return true;
}


void Parser::range() {

}

void Parser::push_operator(Operation::operations op, size_t parameter_count) {
    ops.push(new Operation(op, parameter_count));
}

void Parser::push_nfa_node(nfa_graph *push_node) {
    nfa_graph_nodes.push(push_node);
}

void Parser::collapse_star() {
    auto graph = nfa_graph_nodes.top();
    graph->start->transfer[EPSILON] = graph->end;
    graph->end->transfer[EPSILON] = graph->start;
}

void Parser::collapse_plus() {
    auto graph = nfa_graph_nodes.top();
    graph->end->transfer[EPSILON] = graph->start;
}

void Parser::collapse_option() {
    auto graph = nfa_graph_nodes.top();
    graph->start->transfer[EPSILON] = graph->end;
}

void Parser::collapse_or() {
    if (nfa_graph_nodes.size() < 2) {
        return;
    }
    auto l = nfa_graph_nodes.top();
    nfa_graph_nodes.pop();
    auto r = nfa_graph_nodes.top();
    nfa_graph_nodes.pop();


}

void Parser::collapse_char(char ch) {

}

void Parser::do_concat() {

}


