//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_RESTRING_H
#define FASTREGEXCPP_RESTRING_H


#include <string>

class REstring {
public:
    explicit REstring(const std::string &pattern_) : pattern(pattern_) {}

    void remove_prefix(int count = 1);

    size_t size() const;

    char operator[](const size_t index_) const {
        return index_ + cur_index >= pattern.size() ? pattern[pattern.size() - 1] : pattern[index_ + cur_index];
    }

private:
    const std::string &pattern;
    size_t cur_index = 0;
};


#endif //FASTREGEXCPP_RESTRING_H
