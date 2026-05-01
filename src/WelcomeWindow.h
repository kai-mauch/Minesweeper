#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class WelcomeWindow {
private:
    RenderWindow window;
    Font gameFont;
    string playerName;
    bool nameEntered = false;

    int windowWidth;
    int windowHeight;

    const int MAX_NAME_LENGTH = 10;

    void loadFont();
    void handleInput(Event event);
    string formatName(string name) const;

public:
    WelcomeWindow(int cols, int rows);
    ~WelcomeWindow();

    bool isOpen() const;
    void handleEvents();
    void render();
    string getPlayerName() const;
};

#endif