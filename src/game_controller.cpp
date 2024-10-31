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
        case sf::Event::MouseButtonReleased:
            handleMouseRelease(event.mouseButton.button, mousePosition);
            break;
        case sf::Event::MouseMoved:
            handleMouseMove(mousePosition);
            break;
        case sf::Event::TextEntered:
            handleTextEntered(event.text.unicode);
            break;
        default:
            break;
    }
}

void GameController::handleKeyPress(const sf::Keyboard::Key& code) {
    switch (code) {
        case sf::Keyboard::Key::Left:
        case sf::Keyboard::Key::Right:
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::Down:
            if (model_.getState() == GameModel::State::RUNNING) {
                view_.updateCamera(code);
            } else {
                model_.handleKeyPress(code);
            }
            break;
        case sf::Keyboard::Key::Enter:
            model_.setState();
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
                case GameModel::State::MENU: {
                    if (model_.getMenu(Menu::Type::MAIN).handleMouseClick(mousePosition)) {
                        model_.setState();
                    }
                    break;
                }
                case GameModel::State::RUNNING:
                    // TODO: Implement mouse actions in running state
                    model_.getWorld().getCannon()->startLaunch();
                    break;
                case GameModel::State::GAME_OVER: {
                    if (model_.getMenu(Menu::Type::GAME_OVER).handleMouseClick(mousePosition)) {
                        model_.setState();
                    }
                    break;
                }
                case GameModel::State::PAUSED:
                    // TODO: Implement mouse actions in Pause menu
                    break;
                case GameModel::State::SETTINGS:
                    if (model_.getMenu(Menu::Type::SETTINGS).handleMouseClick(mousePosition)) {
                        model_.setState();
                    }
                    break;
                default:
                    break;
            }
            break;
        case sf::Mouse::Button::Right:
            // TODO: Implement mouse right actions
            if (model_.getState() == GameModel::State::RUNNING) {
                model_.getWorld().resetBird();
            }
            break;
        default:
            break;
    }
}

void GameController::handleMouseRelease(sf::Mouse::Button button, sf::Vector2f mousePosition) {
    if (
        button == sf::Mouse::Button::Left
        && model_.getState() == GameModel::State::RUNNING
        && model_.getWorld().getCannon()->isLaunching()
    ) {
        model_.launchBird();
    }
}

void GameController::handleMouseMove(sf::Vector2f mousePosition) {
    switch(model_.getState()) {
        case GameModel::State::RUNNING:
            model_.rotateCannon(mousePosition);
            break;
        case GameModel::State::MENU:
            model_.getMenu(Menu::Type::MAIN).handleMouseMove(mousePosition);
            break;
        case GameModel::State::GAME_OVER:
            model_.getMenu(Menu::Type::GAME_OVER).handleMouseMove(mousePosition);
            break;
        default:
            break;
    }
}

void GameController::handleTextEntered(sf::Uint32 unicode) {
    model_.handleTextEntered(unicode);
}
