#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

Board::Board() {
    srand(time(0));
    readConfig();  // Read from config.cfg

    // Initialize grid with the dimensions we read
    grid = vector<vector<Tile>>(rows, vector<Tile>(cols));

    generateMines();
    calculateAdjacentMines();
}

void Board::readConfig() {
    ifstream configFile("files/config.cfg");

    // Check if file opened successfully
    if (!configFile.is_open()) {
        // If file doesn't exist, use default values
        cols = 25;
        rows = 16;
        mineCount = 50;
        return;
    }

    // Read the three values from config.cfg
    configFile >> cols;   // Line 1: number of columns
    configFile >> rows;   // Line 2: number of rows
    configFile >> mineCount;  // Line 3: number of mines

    configFile.close();
}

Board::~Board() {
}

int Board::getRows() const {
    return rows;
}

int Board::getCols() const {
    return cols;
}

int Board::getMineCount() const {
    return mineCount;
}

Tile* Board::getTile(int row, int col) {
    if (isValid(row, col)) {
        return &grid[row][col];
    }
    return nullptr;
}

bool Board::isValid(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void Board::generateMines() {
    int minesPlaced = 0;

    // Keep placing mines until we have enough
    while (minesPlaced < mineCount) {
        // Pick a random row (0 to rows-1)
        int row = rand() % rows;

        // Pick a random column (0 to cols-1)
        int col = rand() % cols;

        // If this spot doesn't already have a mine, place one
        if (!grid[row][col].getIsMine()) {
            grid[row][col].setMine(true);
            minesPlaced++;
        }
    }
}

void Board::calculateAdjacentMines() {
    // Check every tile on the board
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // Only care about non-mine tiles
            if (!grid[row][col].getIsMine()) {
                // Count mines around this tile
                int count = countAdjacentMines(row, col);

                // Store that count in the tile
                grid[row][col].setAdjacentMines(count);
            }
        }
    }
}

int Board::countAdjacentMines(int row, int col) const {
    int count = 0;

    // Check all 8 surrounding tiles
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            // Skip the center tile (it's not adjacent to itself)
            if (r == row && c == col) continue;

            // If this tile is valid and has a mine, count it
            if (isValid(r, c) && grid[r][c].getIsMine()) {
                count++;
            }
        }
    }

    return count;
}


void Board::flagTile(int row, int col) {
    if (!isValid(row, col)) return;

    Tile* tile = getTile(row, col);
    if (tile != nullptr) {
        tile->toggleFlag();
    }
}

void Board::reset() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            grid[row][col].reset();
        }
    }

    generateMines();
    calculateAdjacentMines();
}

void Board::revealTile(int row, int col) {
    if (!isValid(row, col)) return;

    Tile* tile = getTile(row, col);
    if (tile == nullptr || tile->getIsRevealed()) return;

    // Reveal this tile
    tile->reveal();

    // If it has 0 adjacent mines, flood-fill the neighbors
    if (tile->getAdjacentMines() == 0) {
        revealTileRecursive(row, col);
    }
}

void Board::revealTileRecursive(int row, int col) {
    // Check all 8 neighbors
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            // Skip center tile
            if (r == row && c == col) continue;

            if (isValid(r, c)) {
                Tile* neighbor = getTile(r, c);

                // If neighbor is hidden, reveal it
                if (neighbor != nullptr && !neighbor->getIsRevealed()) {
                    neighbor->reveal();

                    // If this neighbor also has 0 mines, recurse on it
                    if (neighbor->getAdjacentMines() == 0) {
                        revealTileRecursive(r, c);
                    }
                }
            }
        }
    }
}