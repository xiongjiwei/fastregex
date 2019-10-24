//
// Created by admin on 2019/9/23.
//

#include "fastre.h"
#include "../lexer/parser.h"

void fastre::compile(const std::string& pattern_) {
    REx::REstring restring(pattern_);
    REx::Parser parser(restring);

    parser.regex();
}
