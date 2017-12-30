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

    //Salsa20 salsa("out.txt");
    //salsa.Encode("qb5j3n29n3j47228jruendjuiASjsdas");

    http::server::server s("localhost", "10001", "web/");
    s.run();

    /*vector<unsigned char> chs;
    printBin(1);
    chs.push_back(1);
    printBin(2);
    chs.push_back(2);
    printBin(3);
    chs.push_back(3);
    printBin(4);
    chs.push_back(4);

    uint32_t res = salsa.FourCharToInt(chs);
    printBin(res);
    chs = salsa.IntToFourChar(res);
    printBin(chs[0]);
    printBin(chs[1]);
    printBin(chs[2]);
    printBin(chs[3]);

    printBin(salsa.LeftShift(1, 10));*/
    //salsa.GenKey();
    //for(int i = 0; i < 32; ++i)
    //    cout << ((uint32_t)salsa.key[i]) << " ";

    return 0;
}
