#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Leaderboard.h"
#include "Tile.h"

using namespace sf;

class GameWindow {
private:
    // Game state enum
    enum class GameState {
        Playing,
        Paused,
        Victory,
        Defeat
    };

    // Window and rendering
    RenderWindow window;
    Board board;
    string playerName;

    // Tile size (in pixels)
    const int TILE_SIZE = 32;

    // Tile textures
    Texture tileHiddenTexture;
    Texture tileRevealedTexture;
    Texture flagTexture;
    Texture mineTexture;
    Texture numberTextures[8];  // for numbers 1-8
    Texture faceWinTexture;
    Texture faceLoseTexture;

    // Button textures
    Texture faceHappyTexture;
    Texture debugTexture;
    Texture pauseTexture;
    Texture playTexture;
    Texture leaderboardTexture;

    // Font for text
    Font gameFont;

    // Game state variables
    GameState gameState;
    bool isPaused = false;
    bool debugMode = false;
    int flagsPlaced = 0;
    Clock gameClock;
    int elapsedSeconds = 0;

    // Private helper functions
    void loadTextures();
    void handleMouseClick(int row, int col, Mouse::Button button);
    void drawBoard();
    void drawTile(int row, int col);
    void drawUI();
    void drawMineCounter();
    void drawTimer();
    void drawFaceButton();
    void drawDebugButton();
    void drawPauseButton();
    void drawLeaderboardButton();
    void toggleDebugMode();

    // Game logic helpers
    bool checkVictory();
    void handleVictory();
    void handleDefeat();
    bool isGameOver() const;
    void resetGame();
    void drawLeaderboardOverlay();
    Leaderboard leaderboard;
    bool showingLeaderboard = false;
    bool isNewHighScore = false;
    float pausedTime = 0.0f;

public:
    GameWindow(string name);
    ~GameWindow();

    // Main loop functions
    bool isOpen() const;
    void handleEvents();
    void update();
    void render();


};

#endif