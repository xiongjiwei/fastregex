//
// Created by admin on 2019/10/24.
//
#ifndef FASTREGEXCPP_RE_H
#define FASTREGEXCPP_RE_H
#include <bitset>
namespace REx {
    typedef uint8_t BYTE;
    typedef int vmregister;

    enum INSTRUCTIONS {
        character   = 0x01,         //character char<1>         :whether sp == char
        split       = 0x02,         //split L1<2>, L2<2>        :new thread  pc = L1, pc = L2
        jmp         = 0x03,         //jmp L<2>                  :jump to L

        oneof       = 0x04,         //oneof set<32>             :sp in set
        loopch      = 0x05,         //loop char<1> times<2>     :loop char n times

        loop        = 0x06,         //loop times<2>             :loop instructions
        endloop     = 0x07,         //endloop L1<2>             :endloop sign

        match       = 0x00,         //match                     :end sign
    };

    template<size_t N>
    static BYTE *cast_to_byte(std::bitset<N> bits) {
        if (N == 0) {
            return nullptr;
        }
        size_t length = (N - 1) / 8 + 1;
        BYTE *char_byte = new BYTE[length];

        for (size_t i = 0; i < length; ++i) {
            size_t temp_byte = 0;
            for (int j = 0; j < 8; ++j) {
                temp_byte <<= 1;
                temp_byte += bits[8 * i + j];
            }
            *(char_byte + i) = temp_byte;
        }

        return char_byte;
    }
}

#endif //FASTREGEXCPP_RE_H
