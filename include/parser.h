//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_PARSER_H
#define FASTREGEXCPP_PARSER_H


#include "restring.h"

class Parser {
public:
    explicit Parser(REstring& restring_): restring(restring_){}

    bool regex();
    void exper();
    void term();
    void factor();
    void charset();
    void group();
    void chars();

private:
    REstring& restring;
};


#endif //FASTREGEXCPP_PARSER_H
