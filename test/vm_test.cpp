//
// Created by admin on 2019/10/21.
//

#include <cstring>
#include "catch.hpp"
#define private public
#include "../src/vm/ReVM.h"

TEST_CASE("vm instructions test") {
    SECTION("character instructions") {
        std::string matched_string = "a";
        char program[] = {0x01, 'a', 0x00};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        auto *thread = new REx::Thread(0, 0, 0);
        vm.ins_character(thread);

        CHECK(thread->alive);
        CHECK(thread->PC == 2);
        CHECK(thread->SP == 1);
    }

    SECTION("character instructions") {
        std::string matched_string = "a";
        char program[] = {0x01, 'b', 0x00};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        auto *thread = new REx::Thread(0, 0, 0);
        vm.ins_character(thread);

        CHECK_FALSE(thread->alive);
    }

    SECTION("split instructions") {
        std::string matched_string = "a";
        char program[] = {0x02, 0x03, 0x06};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        auto *first_thread = new REx::Thread(0, 0, 0);
        vm.ins_split(first_thread);
        auto second_thread = vm.running_thread_list.front();

        CHECK(first_thread->PC == 0x03);
        CHECK(second_thread->PC == 0x06);
    }

    SECTION("jmp instructions") {
        std::string matched_string = "a";
        char program[] = {0x03, 0x03, 0x06};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        auto *thread = new REx::Thread(0, 0, 0);
        vm.ins_jmp(thread);

        CHECK(thread->PC == 0x03);
    }

    SECTION("match instructions") {
        std::string matched_string = "aaaaa";
        char program[] = {0x00};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        auto *thread = new REx::Thread(0, 3, 0);
        vm.ins_match(thread);

        CHECK_FALSE(thread->alive);
        REQUIRE(vm.success_thread_list.size() == 1);
        CHECK(vm.success_thread_list.at(0).start == 0);
        CHECK(vm.success_thread_list.at(0).end == 3);
    }

    SECTION("vm") {
        std::string matched_string = "abbbc";
        char program[] = {0x01, 'a', 0x02, 0x05, 0x09, 0x01, 'b', 0x03, 0x02, 0x01, 'c', 0x00};
        REx::ReVM vm = REx::ReVM(matched_string, program);
        vm.start_vm();

        REQUIRE(vm.success_thread_list.size() == 1);
        CHECK(vm.success_thread_list.at(0).start == 0);
        CHECK(vm.success_thread_list.at(0).end == 5);
        CHECK(vm.running_thread_list.empty());
    }
}