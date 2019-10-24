#include <iostream>
#include "src/re/fastre.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    fastre re;
    re.compile(".*");

    return 0;
}