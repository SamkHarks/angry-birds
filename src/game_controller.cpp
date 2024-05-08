#include "game_controller.hpp"


GameController::GameController(GameModel& model, GameView& view) : model_(model), view_(view) {}

void GameController::handleEvents() {
    for (auto event = sf::Event{}; view_.getWindow().pollEvent(event);) {
       handleEvent(event);
    }
}

void GameController::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        view_.getWindow().close();
    }
}
