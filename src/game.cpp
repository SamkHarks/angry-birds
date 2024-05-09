#include "game.hpp"

Game::Game() : model_(), view_(), controller_(model_, view_) {}

void Game::run() {
    while (view_.isOpen()) {
        controller_.handleEvents();
        model_.update();
        view_.render(model_);
    }
}