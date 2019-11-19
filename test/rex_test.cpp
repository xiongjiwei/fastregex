//
// Created by admin on 2019/10/24.
//

#include <bitset>
#include "catch.hpp"
#include <cstring>
#include "../src/re/rex.h"

TEST_CASE("bitset cast to byte test", "[bitset][byte]") {
    SECTION("") {
        std::bitset<256> bitset;
        for (size_t i = 0; i < 256; ++i) {
            bitset[i] = (i & 1) == 0;
        }
        REx::BYTE *ret = REx::cast_to_byte(bitset);
        REx::BYTE should_equal[] = {
                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        };

        REQUIRE(ret != nullptr);
        CHECK(memcmp(ret, should_equal, 32) == 0);
    }

    SECTION("") {
        std::bitset<256> bitset;
        for (size_t i = 0; i < 256; ++i) {
            bitset[i] = (i % 8) == 0;
        }
        REx::BYTE *ret = REx::cast_to_byte(bitset);
        REx::BYTE should_equal[] = {
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        };

        REQUIRE(ret != nullptr);
        CHECK(memcmp(ret, should_equal, 32) == 0);
    }

    SECTION("") {
        std::bitset<256> bitset;
        for (size_t i = 0; i < 256; ++i) {
            bitset[i] = true;
        }
        REx::BYTE *ret = REx::cast_to_byte(bitset);
        REx::BYTE should_equal[] = {
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };

        REQUIRE(ret != nullptr);
        CHECK(memcmp(ret, should_equal, 32) == 0);
    }

    SECTION("") {
        std::bitset<0> bitset;
        REx::BYTE *ret = REx::cast_to_byte(bitset);

        CHECK(ret == nullptr);
    }
}