#include "game.hpp"
#include "world.hpp"

Game::Game() : model_(), view_(), controller_(model_, view_) {}

void Game::run() {
    while (view_.isOpen()) {
        controller_.handleEvents();
        model_.update();
        view_.updateCamera(model_);
        view_.setGameView();
        view_.updateHUD(model_);
        view_.render(model_);
    }
}