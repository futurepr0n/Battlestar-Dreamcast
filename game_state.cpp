#include "game_state.hpp"

// If you want to add any non-inline function definitions, you can do so here.
// For example:

void GameState::setPlayerName(const std::string& name) {
    playerName = name;
}

std::string GameState::getPlayerName() const {
    return playerName;
}

void GameState::addScore(int points) {
    score += points;
}

int GameState::getScore() const {
    return score;
}

void GameState::resetScore() {
    score = 0;
}