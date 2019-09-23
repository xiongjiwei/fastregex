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

void Parser::do_concat(char ch) {
    auto graph = nfa_graph_nodes.top();
    graph->end->transfer[ch] = new NFA();
    graph->end->is_end = false;
    graph->end = graph->end->transfer[ch];
    graph->end->is_end = true;
}


