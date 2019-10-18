//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_NFA_H
#define FASTREGEXCPP_NFA_H


#include <map>
#define EPSILON 0

class NFA {
public:
    std::map<char, NFA *> transfer;
    bool is_end;
};

class Operation {
public:
    auto get_op() {
        return op;
    }

    auto get_parameters_count() { return parameters_count; }

    enum operations {
        OR,
        STAR,
        OPTION,
        PLUS
    };

    Operation(operations op_, size_t parameter_count_) : op(op_), parameters_count(parameter_count_) {}

private:
    operations op;
    size_t parameters_count;
};


typedef struct nfa_graph_ {
    NFA *start;
    NFA *end;
    nfa_graph_(NFA *start_, NFA *end_) : start(start_), end(end_) {}
} nfa_graph;


#endif //FASTREGEXCPP_NFA_H
