#include "game_controller.hpp"


GameController::GameController(GameModel& model, GameView& view, EventQueue& eventQueue) : model_(model), view_(view), eventQueue_(eventQueue) {}

// handle events from queue
void GameController::handleQueueEvents() {
    while (eventQueue_.hasEvents()) {
        Event event = eventQueue_.poll();
        switch (event.getType()) {
            case Event::Type::BirdLaunching:
            case Event::Type::BirdLaunched:
            case Event::Type::ObjectCountUpdated:
            case Event::Type::ScoreUpdated:
            case Event::Type::UpdateHUD:
                view_.updateHUD(model_);
                break;
            default:
                break;
        }
    }
}

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
        case sf::Event::KeyReleased:
            handleKeyRelease(event.key.code);
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
            if (model_.isRunning() || model_.isLevelEditor()) {
                view_.updateCamera(code);
            } else {
                model_.handleKeyPress(code);
            }
            break;
        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::R:
        case sf::Keyboard::Key::T:
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Delete:
            model_.handleKeyPress(code);
            break;
        case sf::Keyboard::Key::P:
        case sf::Keyboard::Key::Escape:
            model_.handleKeyPress(code);
            model_.getMenu<Pause>(Menu::Type::PAUSE).updatePosition(view_);
            break;
        default:
            break;
    }
}

void GameController::handleKeyRelease(const sf::Keyboard::Key& code) {
    switch (code) {
        case sf::Keyboard::Key::R:
        case sf::Keyboard::Key::T:
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::S:
            model_.handleKeyRelease();
            break;
        default:
            break;
    }
}

void GameController::handleMousePress(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePosition) {
    switch (mouseButton) {
        case sf::Mouse::Button::Left:
            model_.handleMouseLeftClick(mousePosition, view_);
            break;
        case sf::Mouse::Button::Right:
            model_.handleMouseRightClick(mousePosition);
            break;
        default:
            break;
    }
}

void GameController::handleMouseRelease(const sf::Mouse::Button& button, const sf::Vector2f& mousePosition) {
    model_.handleMouseRelease(button, mousePosition);
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
    model_.handleResize(view_);
}
