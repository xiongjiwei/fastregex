//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_PARSER_H
#define FASTREGEXCPP_PARSER_H


#include <stack>
#include "restring.h"
#include "nfa.h"

class Parser {
public:
    explicit Parser(REstring& restring_): restring(restring_){}

    bool regex();
    void exper();
    void term();
    void factor();
    void charset();
    void range();
    void group();
    void chars();

    void push_operator(Operation::operations op, size_t parameter_count);
    void push_nfa_node(nfa_graph*);

private:
    REstring& restring;
    std::stack<nfa_graph*> nfa_graph_nodes;
    std::stack<Operation*> ops;

    void collapse_star();
    void collapse_plus();
    void collapse_option();
    void collapse_or();
};


#endif //FASTREGEXCPP_PARSER_H
