#include "game.hpp"
#include "world.hpp"

Game::Game(): model_(), view_(), eventQueue_(), controller_(model_, view_, eventQueue_), eventDispatcher_(std::make_unique<EventDispatcher>(eventQueue_)) {
    model_.setEventDispatcher(eventDispatcher_.get());
    view_.setEventDispatcher(eventDispatcher_.get());
}

void Game::run() {
    while (view_.isOpen()) {
        timer.restart();
        controller_.handleEvents();
        model_.update();
        view_.updateCamera(model_);
        view_.setGameView();
        controller_.handleQueueEvents();
        view_.render(model_);
        controlFrameRate();
    }
}

// For now use simple sleep to control frame rate. Later change to better frame rate control, e.g. interpolation
void Game::controlFrameRate() {
    elapsedTime_ = timer.getElapsedTime().asSeconds();
    if (elapsedTime_ < TIME_STEP) {
        sf::sleep(sf::seconds(TIME_STEP - elapsedTime_));
    }
}