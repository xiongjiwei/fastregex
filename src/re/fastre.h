//
// Created by admin on 2019/9/23.
//

#ifndef FASTREGEXCPP_FASTRE_H
#define FASTREGEXCPP_FASTRE_H


#include <string>
#include "../vm/program.h"

namespace REx {
    class Fastre {
    public:
        void compile(const std::string&);
        bool full_match(const std::string&);

        ~Fastre() {
            delete [] bytecode;
        }
    private:
        BYTE *bytecode = nullptr;
    };
}


#endif //FASTREGEXCPP_FASTRE_H
