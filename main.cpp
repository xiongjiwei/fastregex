#include <iostream>
#include "src/re/fastre.h"

int main() {
    REx::Fastre re;
    re.compile(".*");
    if (re.full_match("09933")) {
        std::cout << "Hello, World!" << std::endl;
    }
    re.release();

    return 0;
}