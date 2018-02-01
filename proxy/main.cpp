#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>

#include <bitset>
#include "Salsa20.h"

#include "server/server.hpp"

using namespace std;

void printBin(uint32_t n) {
    std::cout << std::bitset<sizeof(n) * 8>(n) << std::endl;
}

int main() {
    http::server::server s("localhost", "10002", "web/");
    s.run();
    return 0;
}
