#include <SFML/Graphics.hpp>
#include "WelcomeWindow.h"
#include "GameWindow.h"
#include <fstream>

using namespace std;

int main() {
    // Read board config
    int cols = 25, rows = 16;  // defaults

    ifstream configFile("files/board_config.cfg");
    if (configFile.is_open()) {
        configFile >> cols >> rows;
        configFile.close();
    }

    // Calculate window dimensions
    int welcomeWidth = cols * 32;
    int welcomeHeight = rows * 32 + 100;

    // Show welcome window (get player name)
    WelcomeWindow welcome(welcomeWidth, welcomeHeight);

    while (welcome.isOpen()) {
        welcome.handleEvents();
        welcome.render();
    }

    string playerName = welcome.getPlayerName();

    if (playerName.empty()) {
        return 0;  // User closed without entering name
    }

    // Start the game
    GameWindow game(playerName);

    while (game.isOpen()) {
        game.handleEvents();
        game.update();
        game.render();
    }

    return 0;
}