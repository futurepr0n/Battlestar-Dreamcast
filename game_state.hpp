#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <string>

class GameState {
public:
    static GameState& getInstance() {
        static GameState instance;
        return instance;
    }

    void setPlayerName(const std::string& name);
    std::string getPlayerName() const;

    void addScore(int points);
    int getScore() const;

    void resetScore();

private:
    GameState() : score(0) {}
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    std::string playerName;
    int score;
};

#endif // GAME_STATE_HPP