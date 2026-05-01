#include "WelcomeWindow.h"
#include <iostream>
#include <cctype>

using namespace std;
using namespace sf;

WelcomeWindow::WelcomeWindow(int cols, int rows) {
    // Calculate window dimensions (same as game window)
    windowWidth = cols * 32;
    windowHeight = rows * 32 + 100;

    window.create(VideoMode(Vector2u(windowWidth, windowHeight)), "Minesweeper");
    loadFont();
}

WelcomeWindow::~WelcomeWindow() {
}

bool WelcomeWindow::isOpen() const {
    return window.isOpen();
}

void WelcomeWindow::loadFont() {
    if (!gameFont.openFromFile("files/font.ttf")) {
        cerr << "Error loading font.ttf" << endl;
    }
}

void WelcomeWindow::handleEvents() {
    auto event = window.pollEvent();

    while (event) {
        if (event->is<Event::Closed>()) {
            window.close();
        }

        if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
            handleInput(*event);
        }

        if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
            if (keyEvent->code == Keyboard::Key::Enter) {
                if (!playerName.empty()) {
                    nameEntered = true;
                    window.close();
                }
            } else if (keyEvent->code == Keyboard::Key::Backspace) {
                if (!playerName.empty()) {
                    playerName.pop_back();
                }
            }
        }

        event = window.pollEvent();
    }
}

void WelcomeWindow::handleInput(Event event) {
    if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
        char input = static_cast<char>(textEvent->unicode);

        // Only allow alphabetic characters
        if (isalpha(input) && playerName.length() < MAX_NAME_LENGTH) {
            playerName += input;
        }
    }
}

string WelcomeWindow::formatName(string name) const {
    if (name.empty()) return name;

    // Capitalize first letter, lowercase the rest
    name[0] = toupper(name[0]);
    for (int i = 1; i < name.length(); i++) {
        name[i] = tolower(name[i]);
    }
    return name;
}

void WelcomeWindow::render() {
    window.clear(Color::Blue);

    // Title: "WELCOME TO MINESWEEPER!"
    Text titleText(gameFont, "WELCOME TO MINESWEEPER!", 24);
    titleText.setFillColor(Color::White);
    titleText.setStyle(Text::Bold | Text::Underlined);
    titleText.setPosition(Vector2f(windowWidth / 2 - 200, windowHeight / 2 - 100));
    window.draw(titleText);

    // Prompt: "Enter your name:"
    Text promptText(gameFont, "Enter your name:", 20);
    promptText.setFillColor(Color::White);
    promptText.setPosition(Vector2f(windowWidth / 2 - 120, windowHeight / 2 - 20));
    window.draw(promptText);

    // Player's typed name (formatted)
    string displayName = formatName(playerName);
    Text nameText(gameFont, displayName, 18);
    nameText.setFillColor(Color::Yellow);
    nameText.setPosition(Vector2f(windowWidth / 2 - 80, windowHeight / 2 + 30));
    window.draw(nameText);

    // Cursor
    Text cursorText(gameFont, "|", 18);
    cursorText.setFillColor(Color::Yellow);
    cursorText.setPosition(Vector2f(windowWidth / 2 - 80 + displayName.length() * 10,
                                    windowHeight / 2 + 30));
    window.draw(cursorText);

    window.display();
}

string WelcomeWindow::getPlayerName() const {
    return formatName(playerName);
}