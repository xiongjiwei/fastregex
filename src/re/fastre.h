//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_FASTRE_H
#define FASTREGEXCPP_FASTRE_H


#include <string>
#include <vector>
#include "../vm/program.h"

namespace REx {
    class Fastre {
    public:
        void compile(const std::string&);
        bool full_match(const std::string&);
        std::vector<int> match(const std::string&);

        void release() {
            delete [] bytecode;
            bytecode = nullptr;
        }

        ~Fastre() {
            delete [] bytecode;
            bytecode = nullptr;
        }
    private:
        BYTE *bytecode = nullptr;
        std::string error_msg;
        int error_index;
    };
}


#endif //FASTREGEXCPP_FASTRE_H
