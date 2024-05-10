#include "game_controller.hpp"


GameController::GameController(GameModel& model, GameView& view) : model_(model), view_(view) {}

void GameController::handleEvents() {
    sf::Vector2f mousePosition = view_.mapPixelToCoords(sf::Mouse::getPosition(view_));
    for (auto event = sf::Event{}; view_.pollEvent(event);) {
       handleEvent(event, mousePosition);
    }
}

void GameController::handleEvent(const sf::Event& event, sf::Vector2f mousePosition) {
    switch (event.type) {
        case sf::Event::Closed:
            view_.close();
            break;
        case sf::Event::KeyPressed:
            handleKeyPress(event.key.code);
            break;
        case sf::Event::MouseButtonPressed:
            handleMousePress(event.mouseButton.button, mousePosition);
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

void GameController::handleMousePress(const sf::Mouse::Button& mouseButton, sf::Vector2f mousePosition) {
    switch (mouseButton) {
        case sf::Mouse::Button::Left:
            // TODO: Implement mouse left actions
            switch(model_.getState()) {
                case GameModel::State::MENU:
                    if (mousePosition.x >= 400 && mousePosition.x < 600 && mousePosition.y >= 200 && mousePosition.y < 300) {
                        model_.setState(GameModel::State::RUNNING);
                    }
                    // TODO: Handle rest of the menu buttons
                    break;
                case GameModel::State::RUNNING:
                    // TODO: Implement mouse actions in running state
                    break;
                case GameModel::State::PAUSED:
                    // TODO: Implement mouse actions in Pause menu
                    break;
                case GameModel::State::SETTINGS:
                    // TODO: Implement mouse actions in settings
                    break;
                default:
                    break;
            }
            break;
        case sf::Mouse::Button::Right:
            // TODO: Implement mouse right actions
            break;
        default:
            break;
    }
}
