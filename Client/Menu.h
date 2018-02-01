#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include <curses.h>
#include <vector>

class Menu {
    std::vector<std::string> logo;

    const char items[2][6] = {
        "NAME:",
        "PORT:"
    };

    void LoadLogo() {
        std::ifstream fin("logo.txt");
        std::string s;
        while (std::getline(fin, s)) {
            logo.push_back(s);
        }
        fin.close();
    }

    int row = 0, col = 0;
    unsigned int choice = 0;
public:
    bool isRun = true;
    std::string name = "";
    std::string port = "";

    Menu(int _row, int _col) {
        row = _row;
        col = _col;
        LoadLogo();
    }

    void PrintLogo(int col) {
        for (auto i = 0; i < logo.size(); ++i) {
            mvprintw(i, (col-50)/2, "%s\n", logo[i].c_str());
        }
    }

    void Print() {
        clear();
        PrintLogo(col);
        for ( unsigned i = 0; i < 2; i++ ) {
            if ( i == choice ) {
                mvprintw(17+i, (col-5)/2, "%s>_%s\n", items[i],
                         (choice == 0 ? name.c_str() : port.c_str()));
            } else {
                mvprintw(17+i, (col-5)/2, "%s  %s\n", items[i],
                         (choice == 1 ? name.c_str() : port.c_str()));
            }
        }
    }

    void KeyControl() {
        int key = getch();
        if (key == KEY_UP) {
            if (choice) choice--;
        } else if (key == KEY_DOWN) {
            if (choice != 1) choice++;
        } else if (key == KEY_LEFT) {
            if (choice == 0 && name != "") name = name.substr(0, name.size()-1);
            if (choice == 1 && port != "") port = port.substr(0, port.size()-1);
        } else if (key == 'Q') {
            exit(0);
        } else if (key == '\n') {
            if (port != "" && name != "") {
                isRun = false;
            }
        } else if (key >= '0' && key <= '~') {
            if (choice == 0) {
                name += char(key);
            } else {
                port += char(key);
            }
        }
    }
};

#endif // MENU_H_INCLUDED
