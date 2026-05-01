
#ifndef PROJECT3_BOARD_H
#define PROJECT3_BOARD_H
#include <vector>
#include <string>
#include "Tile.h"
using namespace std;

class Board {
private:
    vector<vector<Tile>> grid;
    int rows;
    int cols;
    int mineCount;

    void generateMines();
    void calculateAdjacentMines();
    bool isValid(int row, int col) const;
    void readConfig();
    void revealTileRecursive(int row, int col);  // helper for flood-fill
public:
    Board();
    ~Board();

    // Getters
    int getRows() const;
    int getCols() const;
    int getMineCount() const;
    Tile* getTile(int row, int col);

    // Game actions
    void revealTile(int row, int col);
    void flagTile(int row, int col);
    void reset();

    // Helper functions
    int countAdjacentMines(int row, int col) const;
};

#endif //PROJECT3_BOARD_H