#include "Tile.h"

//Default Constructor
Tile::Tile() : isMine(false), isRevealed((false), isFlagged(false), adjacentMines(0)){}

bool Tile::getIsMine() const {
    return isMine;
}

bool Tile::getIsRevealed() const {
    return isRevealed;
}

bool Tile::getIsFlagged() const {
    return isFlagged;
}

int Tile::getAdjacentMines() const {
    return adjacentMines;
}

void Tile::setMine(bool mine) {
    isMine = mine;
}

void Tile::reveal() {
    isRevealed = true;
    isFlagged = false;  // Remove flag if revealing
}

void Tile::toggleFlag() { // Switches flagged state back and forth
    if (!isRevealed) {  // Can only flag hidden tiles
        isFlagged = !isFlagged;
    }
}

void Tile::setAdjacentMines(int count) {
    adjacentMines = count;
}

void Tile::reset() {
    isMine = false;
    isRevealed = false;
    isFlagged = false;
    adjacentMines = 0;
}