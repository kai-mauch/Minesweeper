#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cctype>
#include <optional>
#include <string>

using namespace std;

// Centers text at position (x, y)
void setText(sf::Text& text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(
        textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f
    ));
    text.setPosition(sf::Vector2f(x, y));
}

// Makes first letter uppercase and the rest lowercase
string formatName(const string& input) {
    string result = input;

    for (int i = 0; i < result.size(); i++) {
        if (i == 0) {
            result[i] = toupper(result[i]);
        } else {
            result[i] = tolower(result[i]);
        }
    }

    return result;
}

int main() {
    // Open config file
    ifstream configFile("files/config.cfg");
    if (!configFile.is_open()) {
        cout << "Could not open files/config.cfg" << endl;
        return 1;
    }

    int cols, rows, mines;
    configFile >> cols >> rows >> mines;
    configFile.close();

    // Welcome window dimensions
    int width = cols * 32;
    int height = rows * 32 + 100;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(width, height)),
        "Welcome to Minesweeper"
    );
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    if (!font.openFromFile("files/font.ttf")) {
        cout << "Could not load files/font.ttf" << endl;
        return 1;
    }

    // Title text
    sf::Text title(font, "WELCOME TO MINESWEEPER!", 24);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    setText(title, width / 2.0f, height / 2.0f - 150);

    // Prompt text
    sf::Text prompt(font, "Enter your name:", 20);
    prompt.setStyle(sf::Text::Bold);
    prompt.setFillColor(sf::Color::White);
    setText(prompt, width / 2.0f, height / 2.0f - 75);

    // User input text
    string playerName = "";

    sf::Text nameText(font, "|", 18);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(sf::Color::Yellow);
    setText(nameText, width / 2.0f, height / 2.0f - 45);

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            // Close window normally
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }

            // Handle text input
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                char ch = static_cast<char>(textEntered->unicode);

                // Backspace
                if (ch == 8) {
                    if (!playerName.empty()) {
                        playerName.pop_back();
                    }
                }
                // Only letters, max 10 chars
                else if (isalpha(ch) && playerName.size() < 10) {
                    playerName += ch;
                }

                playerName = formatName(playerName);
                nameText.setString(playerName + "|");
                setText(nameText, width / 2.0f, height / 2.0f - 45);
            }

            // Handle Enter key
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    if (!playerName.empty()) {
                        window.close();

                        // For now, just show success in console
                        cout << "Player name entered: " << playerName << endl;

                        // Later, this is where you would open the game window
                        return 0;
                    }
                }
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(title);
        window.draw(prompt);
        window.draw(nameText);
        window.display();
    }

    return 0;
}