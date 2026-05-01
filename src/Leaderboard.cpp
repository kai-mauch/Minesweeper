#include "Leaderboard.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

Leaderboard::Leaderboard() {
    loadScores();
}

void Leaderboard::loadScores() {
    std::ifstream file("files/leaderboard.txt");
    if (!file.is_open()) {
        return;
    }

    float time;
    std::string name;

    while (file >> time >> name) {
        scores.push_back({time, name});
    }

    file.close();
}

void Leaderboard::saveScores() {
    std::ofstream file("files/leaderboard.txt");
    if (!file.is_open()) {
        return;
    }

    // Write only top 5 scores
    for (size_t i = 0; i < scores.size() && i < 5; i++) {
        file << scores[i].time << " " << scores[i].name << "\n";
    }

    file.close();
}

bool Leaderboard::addScore(int time, string name) {
    // Check if it's a high score
    if (scores.size() < MAX_SCORES || time < scores.back().time) {
        scores.push_back({time, name});
        
        // Sort by time (ascending - lower time is better)
        sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.time < b.time;
        });
        
        // Keep only top 5
        if (scores.size() > MAX_SCORES) {
            scores.erase(scores.begin() + MAX_SCORES, scores.end());
        }
        
        saveScores();
        return true;
    }
    
    return false;
}

vector<ScoreEntry> Leaderboard::getScores() const {
    return scores;
}

bool Leaderboard::isHighScore(int time) const {
    if (scores.size() < MAX_SCORES) {
        return true;  // Top 5 not full yet
    }
    return time < scores.back().time;  // Better than worst score
}