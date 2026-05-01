#include "GameWindow.h"
#include <iostream>

using namespace std;
using namespace sf;

GameWindow::GameWindow(string name) : playerName(name), gameState(GameState::Playing) {
    int width = board.getCols() * TILE_SIZE;
    int height = board.getRows() * TILE_SIZE + 100;

    window.create(VideoMode(Vector2u(width, height)), "Minesweeper");
    loadTextures();

    if (!gameFont.openFromFile("files/font.ttf")) {
        cerr << "Error loading font.ttf" << endl;
    }

    gameClock.restart();
}

GameWindow::~GameWindow() {
}

bool GameWindow::isOpen() const {
    return window.isOpen();
}

void GameWindow::loadTextures() {
    if (!tileHiddenTexture.loadFromFile("files/images/tile_hidden.png")) {
        cerr << "Error loading tile_hidden.png" << endl;
    }
    if (!tileRevealedTexture.loadFromFile("files/images/tile_revealed.png")) {
        cerr << "Error loading tile_revealed.png" << endl;
    }
    if (!flagTexture.loadFromFile("files/images/flag.png")) {
        cerr << "Error loading flag.png" << endl;
    }
    if (!mineTexture.loadFromFile("files/images/mine.png")) {
        cerr << "Error loading mine.png" << endl;
    }

    for (int i = 1; i <= 8; i++) {
        string filename = "files/images/number_" + to_string(i) + ".png";
        if (!numberTextures[i-1].loadFromFile(filename)) {
            cerr << "Error loading " << filename << endl;
        }
    }

    // Load button textures
    if (!faceHappyTexture.loadFromFile("files/images/face_happy.png")) {
        cerr << "Error loading face_happy.png" << endl;
    }
    if (!debugTexture.loadFromFile("files/images/debug.png")) {
        cerr << "Error loading debug.png" << endl;
    }
    if (!pauseTexture.loadFromFile("files/images/pause.png")) {
        cerr << "Error loading pause.png" << endl;
    }
    if (!playTexture.loadFromFile("files/images/play.png")) {
        cerr << "Error loading play.png" << endl;
    }
    if (!leaderboardTexture.loadFromFile("files/images/leaderboard.png")) {
        cerr << "Error loading leaderboard.png" << endl;
    }
    if (!faceWinTexture.loadFromFile("files/images/face_win.png")) {
        cerr << "Error loading face_win.png" << endl;
    }
    if (!faceLoseTexture.loadFromFile("files/images/face_lose.png")) {
        cerr << "Error loading face_lose.png" << endl;
    }

}

void GameWindow::handleEvents() {
    auto event = window.pollEvent();

    while (event) {
        if (event->is<Event::Closed>()) {
            window.close();
        }

        if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
            int col = mouseEvent->position.x / TILE_SIZE;
            int row = mouseEvent->position.y / TILE_SIZE;

            handleMouseClick(row, col, mouseEvent->button);
        }

        event = window.pollEvent();
    }
}

void GameWindow::update() {
}

void GameWindow::render() {
    window.clear(Color::Black);
    drawBoard();
    drawUI();

    // Show leaderboard overlay if active
    if (showingLeaderboard) {
        drawLeaderboardOverlay();
    }

    window.display();
}

void GameWindow::drawBoard() {
    for (int row = 0; row < board.getRows(); row++) {
        for (int col = 0; col < board.getCols(); col++) {
            drawTile(row, col);
        }
    }
}

void GameWindow::drawTile(int row, int col) {
    Tile* tile = board.getTile(row, col);
    if (tile == nullptr) return;

    Vector2f position(static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE));

    // FIX 5: When paused or leaderboard is open, show all tiles as tile_revealed.png
    if (isPaused || showingLeaderboard) {
        Sprite sprite(tileRevealedTexture);
        sprite.setPosition(position);
        window.draw(sprite);
        return;
    }

    if (tile->getIsFlagged()) {
        Sprite sprite(flagTexture);
        sprite.setPosition(position);
        window.draw(sprite);
    } else if (!tile->getIsRevealed()) {
        Sprite sprite(tileHiddenTexture);
        sprite.setPosition(position);
        window.draw(sprite);

        // In debug mode, show mines even if hidden
        if (debugMode && tile->getIsMine()) {
            Sprite mineSprite(mineTexture);
            mineSprite.setPosition(position);
            window.draw(mineSprite);
        }
    } else if (tile->getIsMine()) {
        Sprite sprite(mineTexture);
        sprite.setPosition(position);
        window.draw(sprite);
    } else {
        Sprite sprite(tileRevealedTexture);
        sprite.setPosition(position);
        window.draw(sprite);

        if (tile->getAdjacentMines() > 0) {
            Sprite numberSprite(numberTextures[tile->getAdjacentMines() - 1]);
            numberSprite.setPosition(position);
            window.draw(numberSprite);
        }
    }
}

bool GameWindow::isGameOver() const {
    return gameState == GameState::Victory || gameState == GameState::Defeat;
}

bool GameWindow::checkVictory() {
    for (int row = 0; row < board.getRows(); row++) {
        for (int col = 0; col < board.getCols(); col++) {
            Tile* tile = board.getTile(row, col);
            if (tile != nullptr && !tile->getIsMine() && !tile->getIsRevealed()) {
                return false;
            }
        }
    }
    return true;
}

void GameWindow::handleVictory() {
    gameState = GameState::Victory;
    cout << "Victory! All mines avoided!" << endl;

    // AUTO-FLAG all remaining unrevealed mines
    for (int row = 0; row < board.getRows(); row++) {
        for (int col = 0; col < board.getCols(); col++) {
            Tile* tile = board.getTile(row, col);
            if (tile != nullptr && tile->getIsMine() && !tile->getIsRevealed()) {
                tile->toggleFlag();
                flagsPlaced++;
            }
        }
    }

    // SET COUNTER TO 0
    flagsPlaced = board.getMineCount();

    // Check if new high score
    isNewHighScore = leaderboard.isHighScore(elapsedSeconds);

    // Save score
    if (isNewHighScore) {
        leaderboard.addScore(elapsedSeconds, playerName);
    }
}

void GameWindow::handleDefeat() {
    gameState = GameState::Defeat;
    cout << "Defeat! You hit a mine!" << endl;

    for (int row = 0; row < board.getRows(); row++) {
        for (int col = 0; col < board.getCols(); col++) {
            Tile* tile = board.getTile(row, col);
            if (tile != nullptr && tile->getIsMine()) {
                tile->reveal();
            }
        }
    }
}

void GameWindow::handleMouseClick(int row, int col, Mouse::Button button) {
    int clickX = col * TILE_SIZE;
    int clickY = row * TILE_SIZE;

    // FIX 2: When game is over, only face and leaderboard buttons work
    if (isGameOver()) {
        // Face button: ((cols / 2.0) × 32) - 32, 32 × (rows + 0.5)
        float faceX = ((board.getCols() / 2.0f) * 32) - 32;
        float faceY = 32 * (board.getRows() + 0.5f);
        if (clickX >= faceX && clickX <= faceX + 32 && clickY >= faceY && clickY <= faceY + 32) {
            resetGame();
            gameClock.restart();
            elapsedSeconds = 0;
            return;
        }

        // Leaderboard button: (cols × 32) - 176, 32 × (rows + 0.5)
        float leaderX = (board.getCols() * 32) - 176;
        float leaderY = 32 * (board.getRows() + 0.5f);
        if (clickX >= leaderX && clickX <= leaderX + 32 && clickY >= leaderY && clickY <= leaderY + 32) {
            showingLeaderboard = !showingLeaderboard;
        }
        return;  // No other buttons or tiles work when game is over
    }

    // FIX 4: When paused, only play/leaderboard/face buttons work
    if (isPaused && button == Mouse::Button::Left) {
        // Face button
        float faceX = ((board.getCols() / 2.0f) * 32) - 32;
        float faceY = 32 * (board.getRows() + 0.5f);
        if (clickX >= faceX && clickX <= faceX + 32 && clickY >= faceY && clickY <= faceY + 32) {
            resetGame();
            gameClock.restart();
            elapsedSeconds = 0;
            return;
        }

        // Play button (same position as pause button)
        float playX = (board.getCols() * 32) - 240;
        float playY = 32 * (board.getRows() + 0.5f);
        if (clickX >= playX && clickX <= playX + 32 && clickY >= playY && clickY <= playY + 32) {
            isPaused = false;
            gameClock.restart();
            return;
        }

        // Leaderboard button
        float leaderX = (board.getCols() * 32) - 176;
        float leaderY = 32 * (board.getRows() + 0.5f);
        if (clickX >= leaderX && clickX <= leaderX + 32 && clickY >= leaderY && clickY <= leaderY + 32) {
            showingLeaderboard = !showingLeaderboard;
            return;
        }

        // No other buttons or tiles work when paused
        return;
    }

    // Face button: ((cols / 2.0) × 32) - 32, 32 × (rows + 0.5)
    float faceX = ((board.getCols() / 2.0f) * 32) - 32;
    float faceY = 32 * (board.getRows() + 0.5f);
    if (clickX >= faceX && clickX <= faceX + 32 && clickY >= faceY && clickY <= faceY + 32) {
        resetGame();
        gameClock.restart();
        elapsedSeconds = 0;
        return;
    }

    // Debug button: (cols × 32) - 304, 32 × (rows + 0.5)
    float debugX = (board.getCols() * 32) - 304;
    float debugY = 32 * (board.getRows() + 0.5f);
    if (clickX >= debugX && clickX <= debugX + 32 && clickY >= debugY && clickY <= debugY + 32) {
        toggleDebugMode();
        return;
    }

    // FIX 3: Pause button with proper timer handling
    float pauseX = (board.getCols() * 32) - 240;
    float pauseY = 32 * (board.getRows() + 0.5f);
    if (clickX >= pauseX && clickX <= pauseX + 32 && clickY >= pauseY && clickY <= pauseY + 32) {
        if (!isPaused) {
            // Pause: save the current elapsed time
            isPaused = true;
            pausedTime = elapsedSeconds;
        } else {
            // Resume: restart the clock and continue from saved time
            isPaused = false;
            gameClock.restart();
        }
        return;
    }

    // Leaderboard button: (cols × 32) - 176, 32 × (rows + 0.5)
    float leaderboardX = (board.getCols() * 32) - 176;
    float leaderboardY = 32 * (board.getRows() + 0.5f);
    if (clickX >= leaderboardX && clickX <= leaderboardX + 32 && clickY >= leaderboardY && clickY <= leaderboardY + 32) {
        showingLeaderboard = !showingLeaderboard;
        return;
    }

    // If leaderboard is showing, any click closes it
    if (showingLeaderboard) {
        showingLeaderboard = false;
        return;
    }

    // Process tile clicks
    Tile* tile = board.getTile(row, col);
    if (tile == nullptr) return;

    if (button == Mouse::Button::Left) {
        // FIX 1: Left-click on flagged tile has no effect
        if (tile->getIsFlagged()) {
            return;
        }

        if (tile->getIsMine() && !tile->getIsRevealed()) {
            tile->reveal();
            handleDefeat();
            return;
        }

        board.revealTile(row, col);

        if (checkVictory()) {
            handleVictory();
        }
    }

    if (button == Mouse::Button::Right) {
        if (tile != nullptr) {
            if (!tile->getIsFlagged()) {
                flagsPlaced++;
            } else {
                flagsPlaced--;
            }
            board.flagTile(row, col);
        }
    }
}
void GameWindow::resetGame() {
    board.reset();
    gameState = GameState::Playing;
    flagsPlaced = 0;
    isPaused = false;
    pausedTime = 0.0f;
    isNewHighScore = false;
}

void GameWindow::drawUI() {
    drawMineCounter();
    drawTimer();
    drawFaceButton();
    drawDebugButton();
    drawPauseButton();
    drawLeaderboardButton();
}

void GameWindow::drawMineCounter() {
    int remainingMines = board.getMineCount() - flagsPlaced;

    Text mineCountText(gameFont, to_string(remainingMines), 30);
    mineCountText.setFillColor(Color::Red);
    mineCountText.setPosition(Vector2f(32.0f, static_cast<float>(board.getRows() * 32 + 16)));

    window.draw(mineCountText);
}

void GameWindow::drawTimer() {
    // FIX 3: Properly handle pause timing
    if (gameState == GameState::Playing && !isPaused) {
        elapsedSeconds = pausedTime + static_cast<int>(gameClock.getElapsedTime().asSeconds());
    }
    // When paused, elapsedSeconds stays at pausedTime (no update)

    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    string timeStr = to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);

    Text timerText(gameFont, timeStr, 30);
    timerText.setFillColor(Color::Red);
    timerText.setPosition(Vector2f(static_cast<float>(board.getCols() * 32 - 97),
                                   static_cast<float>(board.getRows() * 32 + 16)));

    window.draw(timerText);
}

void GameWindow::drawFaceButton() {
    // Position: ((cols / 2.0) × 32) - 32, 32 × (rows + 0.5)
    float x = ((board.getCols() / 2.0f) * 32) - 32;
    float y = 32 * (board.getRows() + 0.5f);

    // Choose the right face based on game state
    Texture* faceToUse = &faceHappyTexture;
    if (gameState == GameState::Victory) {
        faceToUse = &faceWinTexture;
    } else if (gameState == GameState::Defeat) {
        faceToUse = &faceLoseTexture;
    }

    Sprite faceSprite(*faceToUse);
    faceSprite.setPosition(Vector2f(x, y));
    window.draw(faceSprite);
}

void GameWindow::drawDebugButton() {
    // Position: (cols × 32) - 304, 32 × (rows + 0.5)
    float x = (board.getCols() * 32) - 304;
    float y = 32 * (board.getRows() + 0.5f);

    Sprite debugSprite(debugTexture);
    debugSprite.setPosition(Vector2f(x, y));
    window.draw(debugSprite);
}

void GameWindow::drawPauseButton() {
    // Position: (cols × 32) - 240, 32 × (rows + 0.5)
    float x = (board.getCols() * 32) - 240;
    float y = 32 * (board.getRows() + 0.5f);

    Sprite pauseSprite(isPaused ? playTexture : pauseTexture);
    pauseSprite.setPosition(Vector2f(x, y));
    window.draw(pauseSprite);
}

void GameWindow::drawLeaderboardButton() {
    // Position: (cols × 32) - 176, 32 × (rows + 0.5)
    float x = (board.getCols() * 32) - 176;
    float y = 32 * (board.getRows() + 0.5f);

    Sprite leaderboardSprite(leaderboardTexture);
    leaderboardSprite.setPosition(Vector2f(x, y));
    window.draw(leaderboardSprite);
}

void GameWindow::toggleDebugMode() {
    debugMode = !debugMode;
}

void GameWindow::drawLeaderboardOverlay() {
    // Calculate window dimensions based on board size
    float windowWidth = board.getCols() * 16;
    float windowHeight = (board.getRows() * 16) + 50;

    // Center on screen
    float gameWindowWidth = board.getCols() * TILE_SIZE;
    float gameWindowHeight = board.getRows() * TILE_SIZE + 100;

    float leaderboardX = (gameWindowWidth - windowWidth) / 2;
    float leaderboardY = (gameWindowHeight - windowHeight) / 2;

    // Draw blue background rectangle
    sf::RectangleShape leaderboardBg(Vector2f(windowWidth, windowHeight));
    leaderboardBg.setPosition(Vector2f(leaderboardX, leaderboardY));
    leaderboardBg.setFillColor(sf::Color::Blue);
    window.draw(leaderboardBg);

    // Draw black border
    sf::RectangleShape border(Vector2f(windowWidth, windowHeight));
    border.setPosition(Vector2f(leaderboardX, leaderboardY));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2);
    border.setOutlineColor(sf::Color::Black);
    window.draw(border);

    // Draw "LEADERBOARD" title (bold, underlined, white, 20px) - CENTERED
    Text titleText(gameFont, "LEADERBOARD", 20);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Center the title horizontally
    float titleWidth = titleText.getLocalBounds().size.x;
    float titleX = leaderboardX + (windowWidth / 2) - (titleWidth / 2);
    float titleY = leaderboardY + 10;
    titleText.setPosition(Vector2f(titleX, titleY));
    window.draw(titleText);

    // Get scores from leaderboard
    auto scores = leaderboard.getScores();

    // Draw score entries (white, 18px)
    float contentX = leaderboardX + 20;
    float contentY = leaderboardY + 40;
    float lineHeight = 30;

    for (int i = 0; i < scores.size() && i < 5; i++) {
        int minutes = scores[i].time / 60;
        int seconds = scores[i].time % 60;

        
        string timeStr = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" +
                        (seconds < 10 ? "0" : "") + to_string(seconds);

        string scoreStr = to_string(i + 1) + ".    " + timeStr + "    " + scores[i].name;

        // Add asterisk if new high score
        if (isNewHighScore && gameState == GameState::Victory &&
            scores[i].name == playerName && scores[i].time == elapsedSeconds) {
            scoreStr += "*";
        }

        Text scoreText(gameFont, scoreStr, 18);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setStyle(sf::Text::Bold);
        scoreText.setPosition(Vector2f(contentX, contentY + (i * lineHeight)));
        window.draw(scoreText);
    }
}