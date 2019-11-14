//
// Created by admin on 2019/9/23.
//

#include "fastre.h"
#include "../lexer/parser.h"
#include "../vm/vm.h"

void REx::Fastre::compile(const std::string& pattern_) {
    REx::REstring restring(pattern_);
    REx::Parser parser(restring);

    bytecode = REx::Program::to_bytecode(parser.exper());
}

bool REx::Fastre::full_match(const std::string &string) {
    if (bytecode == nullptr) {
        return false;
    }
    REx::Vm vm = REx::Vm(string, bytecode);
    return vm.do_match(0) == string.size();
}

std::vector<int> REx::Fastre::match(const std::string &string) {
    REx::Vm vm = REx::Vm(string, bytecode);
    std::vector<int> ret(16);
    for (size_t i = 0; i < string.size(); ++i) {
        int sp = vm.do_match(i);
        if (sp != 0) {
            ret.push_back(i);
            ret.push_back(sp);
            i = sp;
        }
    }
    return ret;
}
