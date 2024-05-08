#include "game_model.hpp"

GameModel::GameModel() : state_(State::MENU) {}

void GameModel::update() {
    // TODO: Update game logic here
}

const GameModel::State& GameModel::getState() const {
    return state_;
}