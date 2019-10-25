//
// Created by admin on 2019/10/21.
//

#include <cstring>
#include "catch.hpp"
#define private public
#include "../src/vm/vm.h"

TEST_CASE("vm instructions test") {
    SECTION("character instructions") {
        WHEN("matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x01, 'a', 0x00};
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_character(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 2);
            CHECK(thread->SP == 1);
        }

        WHEN("not matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x01, 'b', 0x00};
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_character(thread);

            CHECK_FALSE(thread->alive);
        }
    }

    SECTION("split instructions") {
        std::string matched_string = "a";
        REx::BYTE program[] = {0x02, 0x00, 0x03, 0x00, 0x06};
        REx::vm vm = REx::vm(matched_string, program);
        auto *first_thread = new REx::Thread(0, 0, 0);
        vm.ins_split(first_thread);
        auto second_thread = vm.running_thread_list.front();

        CHECK(first_thread->PC == 0x03);
        CHECK(second_thread->PC == 0x06);
    }

    SECTION("jmp instructions") {
        std::string matched_string = "a";
        REx::BYTE program[] = {0x03, 0x00, 0x03, 0x00, 0x06};
        REx::vm vm = REx::vm(matched_string, program);
        auto *thread = new REx::Thread(0, 0, 0);
        vm.ins_jmp(thread);

        CHECK(thread->PC == 0x03);
    }

    SECTION("match instructions") {
        std::string matched_string = "aaaaa";
        REx::BYTE program[] = {0x00};
        REx::vm vm = REx::vm(matched_string, program);
        auto *thread = new REx::Thread(0, 3, 0);
        vm.ins_match(thread);

        CHECK_FALSE(thread->alive);
        REQUIRE(vm.success_thread_list.size() == 1);
        CHECK(vm.success_thread_list.at(0).start == 0);
        CHECK(vm.success_thread_list.at(0).end == 3);
    }

    SECTION("loopch instructions") {
        WHEN("matched") {
            std::string matched_string = "aaaaabbb";
            REx::BYTE program[] = {0x05, 'a', 0x00, 0x03};
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_loopch(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 4);
            CHECK(thread->SP == 3);
        }

        WHEN("not matched") {
            std::string matched_string = "aaaaabbb";
            REx::BYTE program[] = {0x05, 'a', 0x00, 0x07};
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_loopch(thread);

            CHECK_FALSE(thread->alive);
            CHECK(vm.running_thread_list.empty());
        }
    }

    SECTION("oneof instructions") {
        WHEN("matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x06,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            };
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 33);
            CHECK(thread->SP == 1);
        }

        WHEN("not matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x06,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK_FALSE(thread->alive);
            CHECK(vm.running_thread_list.empty());
        }

        WHEN("matched") {
            std::string matched_string = "a";
            std::bitset<256> bitset;
            bitset['a'] = true;
            auto bytes = REx::cast_to_byte(bitset);
            REx::BYTE program[33] = {0x06};
            memcpy(program + 1,  bytes, 32);
            delete bytes;
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 33);
            CHECK(thread->SP == 1);
        }

        WHEN("not matched") {
            std::string matched_string = "a";
            std::bitset<256> bitset;
            bitset['a'] = true;
            bitset.flip();
            auto bytes = REx::cast_to_byte(bitset);
            REx::BYTE program[33] = {0x06};
            memcpy(program + 1,  bytes, 32);
            delete bytes;
            REx::vm vm = REx::vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK_FALSE(thread->alive);
        }
    }

    SECTION("vm") {
        std::string matched_string = "abbbc";
        REx::BYTE program[] = {0x01, 'a', 0x02, 0x00, 0x07, 0x00, 0x0c, 0x01, 'b', 0x03, 0x00, 0x02, 0x01, 'c', 0x00};
        REx::vm vm = REx::vm(matched_string, program);
        vm.start_vm();

        REQUIRE(vm.success_thread_list.size() == 1);
        CHECK(vm.success_thread_list.at(0).start == 0);
        CHECK(vm.success_thread_list.at(0).end == 5);
        CHECK(vm.running_thread_list.empty());
    }
}