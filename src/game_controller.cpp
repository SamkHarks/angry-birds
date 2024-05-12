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
            switch (model_.getState()) {
                case GameModel::State::MENU:
                    model_.setMenuSelection(Menu::Type::MAIN, sf::Keyboard::Key::Up);
                    break;
                default:
                    break;
            }
            break;
        case sf::Keyboard::Key::Down:
           // TODO: Implement the moveDown method in the GameModel class
            switch (model_.getState()) {
                case GameModel::State::MENU:
                    model_.setMenuSelection(Menu::Type::MAIN, sf::Keyboard::Key::Down);
                    break;
                default:
                    break;
            }
            break;
        case sf::Keyboard::Key::Enter:
            // TODO: Implement Enter actions for different states
            switch (model_.getState()) {
                case GameModel::State::MENU: {
                    const int selectedItem = model_.getMenu(Menu::Type::MAIN).getSelectedItem();
                    model_.setStateFromMenu(Menu::Type::MAIN, selectedItem);
                    break;
                }
                case GameModel::State::GAME_OVER:
                case GameModel::State::PAUSED:
                case GameModel::State::SETTINGS:
                    // TODO: Handle other menus 
                    break;
                default:
                    break;  
            }
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
                    if (mousePosition.x >= 400 && mousePosition.x <= 560 && mousePosition.y >= 220 && mousePosition.y <= 300) {
                        model_.setState(GameModel::State::RUNNING);
                    } else if (mousePosition.x >= 400 && mousePosition.x <= 665 && mousePosition.y >= 320 && mousePosition.y <= 390) {
                        model_.setState(GameModel::State::SETTINGS);
                    } else if (mousePosition.x >= 400 && mousePosition.x <= 545 && mousePosition.y >= 420 && mousePosition.y <= 480) {
                        model_.setState(GameModel::State::QUIT);
                    }
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
