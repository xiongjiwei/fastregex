//
// Created by admin on 2019/10/21.
//

#include <cstring>
#include "catch.hpp"
#define private public
#include "../src/vm/ReVM.h"

TEST_CASE("vm instructions test") {
    SECTION("character instructions") {
        std::string matched_data = "aaa";
        char program[] = {0x01, 'a', 0x00};
        REx::ReVM vm = REx::ReVM(matched_data, program);
        vm.start_vm();
        auto matched_ret = vm.get_result();
        auto thread_ret = vm.get_thread_info();

        CHECK(thread_ret.empty());
        CHECK(matched_ret.size() == 3);
        CHECK(matched_ret.at(0).start == 0);
        CHECK(matched_ret.at(0).end == 1);
        CHECK(matched_ret.at(1).start == 1);
        CHECK(matched_ret.at(1).end == 2);
        CHECK(matched_ret.at(2).start == 2);
        CHECK(matched_ret.at(2).end == 3);
    }

    SECTION("split instructions") {
        std::string matched_data = "ab";
        char program[] = {0x02, 0x03, 0x06, 0x01, 'a', 0x00, 0x01, 'b', 0x00};
        REx::ReVM vm = REx::ReVM(matched_data, program);
        vm.start_vm();
        auto matched_ret = vm.get_result();
        auto thread_ret = vm.get_thread_info();

        CHECK(thread_ret.empty());
        CHECK(matched_ret.size() == 2);
        CHECK(matched_ret.at(0).start == 0);
        CHECK(matched_ret.at(0).end == 1);
        CHECK(matched_ret.at(1).start == 1);
        CHECK(matched_ret.at(1).end == 2);
    }
}