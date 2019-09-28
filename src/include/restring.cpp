//
// Created by admin on 2019/9/23.
//

#include "restring.h"

void REstring::remove_prefix(int count) {
    cur_index += count;
}

size_t REstring::size() const {
    return pattern.size() >= cur_index ? pattern.size() - cur_index : 0;
}
