//
// Created by admin on 2019/9/23.
//

#include "parser.h"

bool Parser::regex() {
    if (restring.size() > 0) {
        if (restring[0] == '^') {
            restring.remove_prefix(1);
            exper();
            return restring.size() == 1 && restring[0] == '$';
        } else {
            do {
                term();
            } while (restring[0] == '|');
            return true;
        }
    } else {
        return false;
    }
}
