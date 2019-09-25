//
// Created by admin on 2019/9/23.
//

#include "fastre.h"
#include "parser.h"

void fastre::compile(const std::string& pattern_) {
    REstring restring(pattern_);
    Parser parser(restring);

    parser.regex();
}
