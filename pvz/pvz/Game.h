#pragma once
#include <memory>
#include "GameState.h"

class Game {
public:
    void changeState(std::unique_ptr<GameState> newState);
    void run();

private:
    std::unique_ptr<GameState> currentState;
};