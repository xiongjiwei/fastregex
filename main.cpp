#include <iostream>
#include "src/lexer/fastre.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    fastre re;
    re.compile(".*");

    return 0;
}