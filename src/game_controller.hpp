#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "game_model.hpp"
#include "game_view.hpp"

class GameController {
    public:
        GameController(GameModel& model, GameView& view);
        void handleEvents();
    private:
        GameModel& model_;
        GameView& view_;
        // Handle different types of events
        void handleKeyPress(const sf::Keyboard::Key& code);
        void handleKeyRelease(const sf::Keyboard::Key& code);
        void handleMousePress(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePosition);
        void handleMouseRelease(const sf::Mouse::Button& button, const sf::Vector2f& mousePosition);
        void handleMouseMove(const sf::Vector2f& mousePosition);
        void handleTextEntered(const sf::Uint32& unicode);
        void handleResize(const sf::Event& event);
        void handleEvent(const sf::Event& event, const sf::Vector2f& mousePosition);

};

#endif // GAME_CONTROLLER_HPP