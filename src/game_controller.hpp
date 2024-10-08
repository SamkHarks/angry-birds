#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "game_model.hpp"
#include "game_view.hpp"

class GameController {
    public:
        GameController(GameModel& model, GameView& view);
        void handleEvent(const sf::Event& event, sf::Vector2f mousePosition);
        void handleEvents();
        void handleKeyPress(const sf::Keyboard::Key& code);
        void handleMousePress(const sf::Mouse::Button& mouseButton, sf::Vector2f mousePosition);
        void handleMouseRelease(sf::Mouse::Button button, sf::Vector2f mousePosition);
        void handleMouseMove(sf::Vector2f mousePosition);
        void handleTextEntered(sf::Uint32 unicode);

    private:
        GameModel& model_;
        GameView& view_;
};

#endif // GAME_CONTROLLER_HPP