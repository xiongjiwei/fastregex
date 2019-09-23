//
// Created by admin on 2019/9/23.
//

#include "fastre.h"
#include "nfa.h"
#include "parser.h"

void fastre::compile(const std::string& pattern_) {
    Parser parser(pattern_);

    parser.regex();
}
