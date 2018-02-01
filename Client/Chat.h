#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

#include <boost/asio.hpp>
#include "Salsa20.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <curses.h>

#include "Base64.h"

class Chat {
    int row = 0, col = 0;
    std::clock_t start;
    std::vector<std::string> text;
    std::string mes = "";
    int line = 0;
public:
    std::string key = "";
    int id = -1;
    int port = 0;
    std::string name = "";
    bool isRun = true;

    void LoadKey() {
        std::ifstream fin("client.txt");
        if (!fin.is_open()) {
            //cout << "Open Error!\n";
            exit(-1);
        }
        fin >> id;
        fin >> key;
        fin.close();
    }

    Chat(int _row, int _col) {
        row = _row;
        col = _col;
        LoadKey();

        start = std::clock();
    }

    void PrintBorder() {
        mvprintw(0,     0,     "+");
        mvprintw(0,     col-1, "+");
        mvprintw(row-1, 0,     "+");
        mvprintw(row-1, col-1, "+");

        for (auto i = 1; i < col-1; ++i) {
            mvprintw(0, i, "-");
            mvprintw(row-1, i, "-");
            mvprintw(row-3, i, "=");
        }

        for (auto i = 1; i < row-1; ++i) {
            mvprintw(i, 0, "|");
            mvprintw(i, col-1, "|");
        }
    }

    void SetText(std::string all) {
        text.clear();
        std::stringstream ss(all);
        std::string to;
        if (all != "") {
            while(std::getline(ss, to, '\n')) {
                int start = to.find("<p>");
                int end = to.find("</p>");
                if (start == to.npos || end == to.npos) {
                    continue;
                }
                std::string _to = to.substr(start+3, (end - start - 3));
                text.push_back(_to);
            }
        }
    }

    PrintText() {
        int __y = 1;
        for (int i = line; i < line + row-5; ++i) {
            if (i < text.size()) {
                mvprintw(__y, 1, "%s", text[i].c_str());
                __y++;
            }
        }
    }

    void Print() {
        clear();
        PrintBorder();
        mvprintw(row-2, 1, ">_%s", mes.c_str());
        PrintText();
    }

    void KeyControl() {

    double duration;
        double sec = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        if (sec > 5) {
            start = std::clock();
            UpdeteText(Request(false));
        }


        int key = getch();
        if (key == KEY_UP) {
            if (line) line--;
        } else if (key == KEY_DOWN) {
            if (line != text.size()-1) line++;
        } else if (key == KEY_LEFT) {
            if (mes != "") mes = mes.substr(0, mes.size()-1);
        } else if (key == 'Q') {
            exit(0);
        } else if (key == '\n') {
            if (mes != "") {
                UpdeteText(Request(true));
                mes = "";
                //Send mess
                //isRun = false;
            }
        } else if (key >= ' ' && key <= '~') {
            mes += char(key);
        }
    }

    std::string Request(bool needSend) {
        try {
            boost::asio::ip::tcp::iostream s;
            s.expires_from_now(boost::posix_time::seconds(60));
            s.connect("localhost", std::to_string(port));
            if (!s) {
                std::cout << "Unable to connect: " << s.error().message() << "\n";
                exit(-1);
            }


            std::string sndMsg = "";
            if (needSend) {
                /*for (auto i = 0; i < mes.size(); ++i) {
                    if (mes[i] == ' ') {
                        mes[i] = '_';
                    }
                }*/
                std::string ecstr = name + ">_" + mes;
                Salsa20 salsaMsg(ecstr, SalsaText::SALSA_STRING);
                ecstr = salsaMsg.Encode(key);
                ecstr = base64_encode(ecstr.c_str(), ecstr.size());
                sndMsg = "&t=" + ecstr;
            }

            //std::string log = "/index.html?id="+std::to_string(id) + sndMsg;
            //mes = log;
            //mvprintw(10, 10, "%s", log.c_str());


            s << "GET " << "/index.html?id="+std::to_string(id) + sndMsg << " HTTP/1.0\r\n";
            s << "Host: " << "localhost:" + std::to_string(port) << "\r\n";
            s << "Accept: */*\r\n";
            s << "Connection: close\r\n\r\n";
            //s.flush();
            std::string header(std::istreambuf_iterator<char>(s), {});

            Salsa20 salsa(header, SalsaText::SALSA_STRING);
            std::string res = salsa.Encode(key);
            //cout << res << "\n";
            return res;
        } catch (std::exception& e) {
            //mvprintw(10, 10, ">_%s", e.what());
        }
        return "";
    }

    void UpdeteText(std::string in) {
        int start = in.find("<body>");
        int end = in.find("</body>");
        if (start == in.npos || end == in.npos) {
            return;
        }
        std::string t = in.substr(start+6, (end - start - 6));
        SetText(t);
    }
};

#endif // CHAT_H_INCLUDED
