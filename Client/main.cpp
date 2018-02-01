#include "Menu.h"
#include "Chat.h"

using namespace std;
using boost::asio::ip::tcp;

int row, col;
int main() {
    initscr();
    curs_set(0);
    timeout(0);
    keypad(stdscr, true);
    getmaxyx(stdscr, row, col);

    Menu menu(row, col);

    while (menu.isRun) {
        menu.Print();
        menu.KeyControl();
    }

    Chat chat(row, col);
    chat.name = menu.name;
    chat.port = std::stoi(menu.port);
    while (chat.isRun) {
        chat.Print();
        chat.KeyControl();
    }
    endwin();
    return 0;
}




