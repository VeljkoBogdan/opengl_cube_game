#include <iostream>

#include "app/Game.h"

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}