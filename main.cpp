#include <iostream>
#include "src/re/fastre.h"
#include "fastre.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    REx::Fastre re;
    re.compile(".*");

    return 0;
}