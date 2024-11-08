#include "game_controller.hpp"


GameController::GameController(GameModel& model, GameView& view) : model_(model), view_(view) {}

void GameController::handleEvents() {
    const sf::Vector2f& mousePosition = view_.mapPixelToCoords(sf::Mouse::getPosition(view_));
    for (auto event = sf::Event{}; view_.pollEvent(event);) {
       handleEvent(event, mousePosition);
    }
}

void GameController::handleEvent(const sf::Event& event, const sf::Vector2f& mousePosition) {
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
        case sf::Event::Resized:
            handleResize(event);
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

void GameController::handleMousePress(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePosition) {
    switch (mouseButton) {
        case sf::Mouse::Button::Left:
            model_.handleMouseLeftClick(mousePosition);
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

void GameController::handleMouseRelease(const sf::Mouse::Button& button, const sf::Vector2f& mousePosition) {
    if (
        button == sf::Mouse::Button::Left
        && model_.getState() == GameModel::State::RUNNING
        && model_.getWorld().getCannon()->isLaunching()
    ) {
        model_.launchBird();
    }
}

void GameController::handleMouseMove(const sf::Vector2f& mousePosition) {
    model_.handleMouseMove(mousePosition);
}

void GameController::handleTextEntered(const sf::Uint32& unicode) {
    model_.handleTextEntered(unicode);
}

void GameController::handleResize(const sf::Event& event) {
    float width = static_cast<float>(event.size.width);
    float height = static_cast<float>(event.size.height);
    if (width == VIEW.getWidth() && height == VIEW.getHeight()) {
        return; // Ignore resize event on startup
    }
    view_.handleResize(width, height);
    model_.handleResize();
}
