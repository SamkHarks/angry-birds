#include "game_controller.hpp"


GameController::GameController(GameModel& model, GameView& view) : model_(model), view_(view) {}

void GameController::handleEvents() {
    for (auto event = sf::Event{}; view_.pollEvent(event);) {
       handleEvent(event);
    }
}

void GameController::handleEvent(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::Closed:
            view_.close();
            break;
        case sf::Event::KeyPressed:
            handleKeyPress(event.key.code);
            break;
        default:
            break;
    }
}

void GameController::handleKeyPress(const sf::Keyboard::Key& code) {
    switch (code) {
        case sf::Keyboard::Key::Left:
            // TODO: Implement the moveLeft method in the GameModel class
            break;
        case sf::Keyboard::Key::Right:
            // TODO: Implement the moveRight method in the GameModel class
            break;
        case sf::Keyboard::Key::Up:
            // TODO: Implement the moveUp method in the GameModel class
            break;
        case sf::Keyboard::Key::Down:
           // TODO: Implement the moveDown method in the GameModel class
            break;
        default:
            break;
    }
}
