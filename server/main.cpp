#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <cstdio>

#include <bitset>
#include "Salsa20.h"
#include <thread>
#include "server/server.hpp"

using namespace std;

void printBin(uint32_t n) {
    std::cout << std::bitset<sizeof(n) * 8>(n) << std::endl;
}

void PrintClients() {
    for(auto& item : userKeys) {
        cout << item.first << " : " << item.second << endl; //Вывод ключей и значений
    }
    cout << endl;
}

void LoadClients() {
    ifstream fin("clients.txt");
    if (!fin.is_open()) {
        cout << "Open Error!\n";
        exit(-1);
    }
    int id = -1;
    std::string key = "";
    while (fin >> id) {
        fin >> key;
        userKeys[id] = key;
    }
    fin.close();
}

void SaveClients() {
    ofstream fout("clients.txt");
    for(auto& item : userKeys) {
        fout << item.first << " " << item.second << "\n";
    }
    fout.close();
}

void AddClient() {
    Salsa20 getKey;
    std::string newKey = getKey.GenKey(false);
    int max = 0;
    for(auto& item : userKeys) {
        if (max < item.first) {
            max = item.first;
        }
    }
    max++;
    userKeys[max] = newKey;
    ofstream fout("client.txt");
    fout << max << " " << newKey;
    fout.close();
}

void DeleteClient(int id) {
    userKeys.erase(id);
}

void ControlClients() {
    LoadClients();
    while (true) {
        char key = getchar();
        if (key == 'Q') {
            SaveClients();
            exit(0);
        } else if (key == 'a') {
            cout << "New client info in client.txt\n";
            AddClient();
            SaveClients();
        } else if (key == 'd') {
            int client;
            cout << "Enter client id: ";
            cin >> client;
            DeleteClient(client);
            SaveClients();
        } else if (key == 'p') {
            PrintClients();
        }
    }
}

int main() {
    std::thread thr(ControlClients);
    thr.detach();
    //Salsa20 salsa("out.txt");
    //salsa.Encode("qb5j3n29n3j47228jruendjuiASjsdas");

    http::server::server s("localhost", "10001", "web/");
    s.run();

    return 0;
}
