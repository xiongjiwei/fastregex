//
// Created by jwxiong on 2019/11/14.
//

#include "catch.hpp"
#define private public
#include "../src/re/fastre.h"

TEST_CASE("match test") {
    SECTION("any") {
        std::string pattern;
        REx::Fastre fastre;
        WHEN("") {
            pattern = "[a-z]*(abc)+(a|bc)?";
            fastre.compile(pattern);

            CHECK(fastre.full_match("abcdabcabcbc"));
            CHECK_FALSE(fastre.full_match("A"));
        }

        WHEN("") {
            pattern = "\\d*[123]";
            fastre.compile(pattern);

            CHECK(fastre.full_match("553"));
            CHECK(fastre.full_match("55223"));
            CHECK(fastre.full_match("55123"));
            CHECK_FALSE(fastre.full_match("5"));
        }

        WHEN("") {
            pattern = "\\d{4,6}";
            fastre.compile(pattern);

            CHECK(fastre.full_match("55331"));
            CHECK_FALSE(fastre.full_match("512"));
            CHECK_FALSE(fastre.full_match("512s"));
        }

        WHEN("") {
            pattern = "a{4}a";
            fastre.compile(pattern);

            CHECK(fastre.full_match("aaaaa"));
        }

        WHEN("") {
            pattern = ".*";
            fastre.compile(pattern);

            CHECK(fastre.full_match("5533"));
            CHECK(fastre.full_match("512"));
            CHECK(fastre.full_match("512s"));
        }

        WHEN("") {
            pattern = ".{12,90}";
            fastre.compile(pattern);

            CHECK(fastre.full_match("512oidoixlcvlkglduoiuzsiouc"));
            CHECK_FALSE(fastre.full_match("5533"));
        }

        WHEN("") {

        }
    }


    SECTION("wrong regex") {
        std::string pattern;
        REx::Fastre fastre;
        WHEN("bad escape") {
            pattern = "\\";
            fastre.compile(pattern);

            CHECK(fastre.bytecode == nullptr);
            CHECK(fastre.error_index == 1);
            CHECK(fastre.error_msg == "bad escape");
        }

        WHEN("bad square bracket") {
            pattern = "abcg[1-3";
            fastre.compile(pattern);

            CHECK(fastre.bytecode == nullptr);
            CHECK(fastre.error_index == 8);
            CHECK(fastre.error_msg == "bad square bracket");
        }
    }

    SECTION("regex test") {
        std::string pattern;
        REx::Fastre fastre;
        WHEN("ip test") {
            pattern = R"((25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d))";
            fastre.compile(pattern);
            WHEN("right ip") {
                std::string ips[] = {
                        "192.180.200.255",
                        "1.1.2.2",
                        "192.80.0.5",
                        "92.18.20.2",
                        "0.0.0.0",
                };
                for (auto &i : ips) {
                    CHECK(fastre.full_match(i));
                }
            }

            WHEN("wrong ip") {
                std::string ips[] = {
                        "192.180.2010.255",
                        "255.256.200.255",
                        "192.s.200.255",
                        "192.180.2f0.255",
                        "192.180.255",
                        "192.180v200.255",
                        "192.180.200.255.",
                        "192.180.200.255.200",
                        "192.",
                        "abcg",
                };

                for (auto &i : ips) {
                    CHECK_FALSE(fastre.full_match(i));
                }
            }
        }

        WHEN("email test") {
            pattern = R"(\w[-\w\d.+]*@([A-Za-z0-9][-A-Za-z0-9]+\.)+[A-Za-z]{2,14})";
            fastre.compile(pattern);
            WHEN("right email address") {
                std::string emails[] = {
                        "dfq123.@fgdg1-fggv.sdf.com",
                        "d.@fgdg1-fggv.sdf.com.cn.edu.xjw",
                        "d+++.@fggv.sdf.com.cn.edu.xjw",
                };
                for (auto &i : emails) {
                    CHECK(fastre.full_match(i));
                }
            }

            WHEN("wrong email address") {
                std::string emails[] = {
                        "dfq123.@fgdg1-fggv.sdf.com.",
                        "df@q123.@fgdg1-fggv.sdf.com.",
                        "123.@fgdg1-fggv.sdf.com.",
                        "123om.",
                        "d123@om.com.",
                        "d123@om.com.s",
                };

                for (auto &i : emails) {
                    CHECK_FALSE(fastre.full_match(i));
                }
            }
        }

        WHEN("username test") {
            pattern = R"([a-zA-Z]{1}([a-zA-Z0-9]|[._]){4,19})";
            fastre.compile(pattern);
            WHEN("right username") {
                std::string emails[] = {
                        "a.sdfiuoiuo124",
                        "a.sdfi_iu..24",
                        "IU.sdfi_iu..24",
                        "U...........",
                        "U......_____",
                };
                for (auto &i : emails) {
                    CHECK(fastre.full_match(i));
                }
            }

            WHEN("wrong username") {
                std::string emails[] = {
                        ".asdioufos",
                        "f.asd@ioufos",
                        "@.asdioufos",
                        "I.asdioufoasfsdfqwdfdfsdfs",
                        "_.fqwdfdfsdfs",
                };

                for (auto &i : emails) {
                    CHECK_FALSE(fastre.full_match(i));
                }
            }
        }

        WHEN("url test") {
            pattern = R"(https?://([\w]+\.)+[\w]+(/[\w./?%&=]*)?)";
            fastre.compile(pattern);
            WHEN("right urls") {
                std::string emails[] = {
                        "http://www.baidu.com",
                        "https://www.baidu.com/",
                        "https://www.baidu.com////////////",
                        "https://www.baidu.com/sdf.?=&",
                        "https://www.baidu/sdf.?=&",
                };
                for (auto &i : emails) {
                    CHECK(fastre.full_match(i));
                }
            }

            WHEN("wrong urls") {
                std::string emails[] = {
                        "sdfii",
                        "http://www./sdfsdf",
                        "httpss://www.ddd/sdfsdf",
                };

                for (auto &i : emails) {
                    CHECK_FALSE(fastre.full_match(i));
                }
            }
        }
    }
}