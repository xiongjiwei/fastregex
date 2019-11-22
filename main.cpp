#include <iostream>
#include "src/re/fastre.h"

int main() {
    REx::Fastre re;
    std::cout << "Hello World" << std::endl;
    re.compile(R"((25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d)\.(25[0-5]|2[0-4]\d|[0-1]\d\d|[1-9]?\d))");
    std::string ips[] = {
            "192.180.200.255",
            "1.1.2.2",
            "192.80.0.5",
            "92.18.20.2",
            "0.0.0.0",
    };
    for (auto &i : ips) {
        std::cout<<re.full_match(i)<<std::endl;
    }

    return 0;
}