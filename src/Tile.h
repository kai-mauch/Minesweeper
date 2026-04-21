#ifndef TILE_H
#define TILE_H

class Tile {
private:
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int adjacentMines;
public:
    Tile();

    //Getters
    bool getIsMine() const;
    bool getIsRevealed() const;
    bool getIsFlagged() const;
    int getAdjacentMines() const;

    // Setters
    void setMine(bool mine);
    void reveal();
    void toggleFlag();
    void setAdjacentMines(int count);
    void reset();  // Reset tile to hidden state

};

#endif