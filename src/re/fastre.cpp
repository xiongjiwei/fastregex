//
// Created by admin on 2019/9/23.
//

#include "fastre.h"
#include "../lexer/parser.h"
#include "../vm/vm.h"

void REx::Fastre::compile(const std::string& pattern_) {
    REx::REstring restring(pattern_);
    REx::Parser parser(restring);

    auto program_ = new Program();
    bytecode = program_->to_bytecode(parser.exper());
}

bool REx::Fastre::full_match(const std::string &string) {
    if (bytecode == nullptr) {
        return false;
    }
    REx::Vm vm = REx::Vm(string, bytecode);
    vm.start_vm();
    return !vm.get_matched_result().empty() &&
           (vm.get_matched_result()[0].start == 0 && vm.get_matched_result()[0].end == string.size());
}

void REx::Fastre::release() {
    delete [] bytecode;
}
