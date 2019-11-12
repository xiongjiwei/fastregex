//
// Created by admin on 2019/10/21.
//

#include <cstring>
#include "catch.hpp"
#define private public
#include "../src/vm/vm.h"
#include "../src/lexer/parser.h"
#include "../src/re/fastre.h"

TEST_CASE("vm instructions test") {
    SECTION("character instructions") {
        WHEN("matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x01, 'a', 0x00};
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_character(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 2);
            CHECK(thread->SP == 1);
        }

        WHEN("not matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x01, 'b', 0x00};
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_character(thread);

            CHECK_FALSE(thread->alive);
        }
    }

    SECTION("split instructions") {
        std::string matched_string = "a";
        REx::BYTE program[] = {0x02, 0x00, 0x03, 0x00, 0x06};
        int16_t line_num = 3;
        memcpy(program + 1, &line_num, 2);
        line_num = 6;
        memcpy(program + 3, &line_num, 2);
        REx::Vm vm = REx::Vm(matched_string, program);
        auto *first_thread = new REx::Thread(0, 0, 0);
        vm.ins_split(first_thread);
        auto second_thread = vm.get_next_thread();

        CHECK(first_thread->PC == 0x03);
        CHECK(second_thread->PC == 0x06);
    }

    SECTION("jmp instructions") {
        std::string matched_string = "a";
        REx::BYTE program[] = {0x03, 0x00, 0x03, 0x00, 0x06};
        int16_t line_num = 3;
        memcpy(program + 1, &line_num, 2);
        REx::Vm vm = REx::Vm(matched_string, program);
        auto *thread = new REx::Thread(0, 0, 0);
        vm.ins_jmp(thread);

        CHECK(thread->PC == 0x03);
    }

    SECTION("match instructions") {
        std::string matched_string = "aaaaa";
        REx::BYTE program[] = {0x00};
        REx::Vm vm = REx::Vm(matched_string, program);
        auto *thread = new REx::Thread(0, 3, 0);
        vm.ins_match(thread);

        CHECK_FALSE(thread->alive);
        REQUIRE(vm.success_recorder.size() == 1);
        CHECK(vm.success_recorder.at(0).start == 0);
        CHECK(vm.success_recorder.at(0).end == 3);
    }

    SECTION("loopch instructions") {
        WHEN("matched") {
            std::string matched_string = "aaaaabbb";
            REx::BYTE program[] = {0x05, 'a', 0x00, 0x03};
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);

            int16_t line = 3;
            memcpy(program + 2, &line, 2);
            vm.ins_loopch(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 4);
            CHECK(thread->SP == 3);
        }

        WHEN("not matched") {
            std::string matched_string = "aaaaabbb";
            REx::BYTE program[] = {0x05, 'a', 0x00, 0x07};
            int16_t times = 7;
            memcpy(program + 2, &times, 2);
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_loopch(thread);

            CHECK_FALSE(thread->alive);
            CHECK(vm.running_thread_list.empty());
        }
    }

    SECTION("oneof instructions") {
        WHEN("matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x04,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            };
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 33);
            CHECK(thread->SP == 1);
        }

        WHEN("not matched") {
            std::string matched_string = "a";
            REx::BYTE program[] = {0x04,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };
            REx::Vm vm = REx::Vm(matched_string, program);
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
            REx::Vm vm = REx::Vm(matched_string, program);
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
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_oneof(thread);

            CHECK_FALSE(thread->alive);
        }
    }

    SECTION("loop") {
        WHEN("match") {
            std::string matched_string = "a";
            REx::BYTE program[] = {
                    0x06, 0x00, 0x00,
            };
            int16_t line_num = 5;
            memcpy(program + 1, &line_num, 2);
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.ins_loop(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == 3);
            CHECK(vm.loop_times == 5);
        }

        WHEN("endloop") {
            std::string matched_string = "a";
            REx::BYTE program[] = {
                    0x07, 0x00, 0x00,
            };
            int16_t line_num = -2;
            memcpy(program + 1, &line_num, 2);
            REx::Vm vm = REx::Vm(matched_string, program);
            auto *thread = new REx::Thread(0, 0, 0);
            vm.loop_times = 5;
            vm.ins_endloop(thread);

            CHECK(thread->alive);
            CHECK(thread->PC == -2);
            CHECK(vm.loop_times == 4);
        }
    }

    SECTION("vm") {
        WHEN("") {
            std::string matched_string = "abbbc";
            REx::BYTE program[] = {0x01, 'a', 0x02, 0x00, 0x05, 0x00, 0x0a, 0x01, 'b', 0x03, 0x80, 0x07, 0x01, 'c', 0x00};
            int16_t line_num = 5;
            memcpy(program + 3, &line_num, 2);
            line_num = 10;
            memcpy(program + 5, &line_num, 2);
            line_num = -7;
            memcpy(program + 10, &line_num, 2);
            REx::Vm vm = REx::Vm(matched_string, program);
            vm.start_vm();

            REQUIRE(vm.success_recorder.size() == 1);
            CHECK(vm.success_recorder.at(0).start == 0);
            CHECK(vm.success_recorder.at(0).end == 5);
            CHECK(vm.running_thread_list.empty());
        }

        WHEN("") {
            std::string matched_string = "aaaaa";
            REx::BYTE program[] = {
                    0x06, 0x00, 0x00,
                    0x01, 'a',
                    0x07, 0x00, 0x00,
                    0x00
            };
            int16_t line_num = 5;
            memcpy(program + 1, &line_num, 2);
            line_num = -2;
            memcpy(program + 6, &line_num, 2);
            REx::Vm vm = REx::Vm(matched_string, program);
            vm.start_vm();

            REQUIRE(vm.success_recorder.size() == 1);
            CHECK(vm.success_recorder.at(0).start == 0);
            CHECK(vm.success_recorder.at(0).end == 5);
            CHECK(vm.running_thread_list.empty());
        }
    }
}

TEST_CASE("match test") {
    SECTION("") {
        std::string string = "[a-z]*(abc)+(a|bc)?";
        REx::Fastre fastre;
        fastre.compile(string);

        CHECK(fastre.full_match("abcdabcabcbc"));
        CHECK_FALSE(fastre.full_match("A"));
    }

    SECTION("") {
        std::string string = "\\d*[123]";
        REx::Fastre fastre;
        fastre.compile(string);

        CHECK(fastre.full_match("553"));
        CHECK_FALSE(fastre.full_match("5"));
    }

    SECTION("") {
        std::string string = "\\d{4,6}";
        REx::Fastre fastre;
        fastre.compile(string);

        CHECK(fastre.full_match("5533"));
        CHECK_FALSE(fastre.full_match("512"));
        CHECK_FALSE(fastre.full_match("512s"));
    }
}