#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>

using namespace std;

struct ScoreEntry {
    int time;      // Time in seconds
    string name;   // Player name
};

class Leaderboard {
private:
    vector<ScoreEntry> scores;  // Max 5 entries
    const int MAX_SCORES = 5;
    const string FILENAME = "files/leaderboard.txt";

public:
    Leaderboard();

    // Load scores from file
    void loadScores();

    // Save scores to file
    void saveScores();

    // Add a new score (returns true if it's in top 5)
    bool addScore(int time, string name);

    // Get all scores
    vector<ScoreEntry> getScores() const;

    // Check if a score beats the 5th place
    bool isHighScore(int time) const;
};

#endif