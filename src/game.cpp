#include "game.hpp"

Game::Game() : model_(new GameModel()), view_(new GameView()), controller_(new GameController(*model_, *view_)) {}

Game::~Game() {
    delete model_;
    delete view_;
    delete controller_;
}

void Game::run() {
    while (view_->isOpen()) {
        controller_->handleEvents();
        model_->update();
        view_->render(model_->getState());
    }
}