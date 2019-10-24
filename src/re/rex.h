//
// Created by admin on 2019/10/24.
//

#include <cstdint>

namespace REx {
    typedef uint8_t BYTE;

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
