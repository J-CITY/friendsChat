#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string>

#include "Salsa20.h"

using namespace std;

string KEY = "qb5j3n29n3j47228jruendjuiASjsdas";

int main() {
    boost::asio::ip::tcp::iostream stream;
    stream.expires_from_now(boost::posix_time::seconds(60));
    stream.connect("localhost", std::to_string(10002));
    stream << "GET /index.html?id=0 HTTP/1.0\r\n";
    stream << "Host: localhost:10002\r\n";
    stream << "Accept: */*\r\n";
    stream << "Connection: close\r\n\r\n";
    stream.flush();
    std::ostringstream ss;
    ss << stream.rdbuf();
    string str = ss.str();

    cout << str << "\n";
    str = str.substr(64);
    //cout << "!!!" << str << "!!!\n";
    cout << "Decrypt...\n";
    Salsa20 salsa(str, Salsa20::SALSA_STRING);
    salsa.Encode(KEY);
    return 0;
}

