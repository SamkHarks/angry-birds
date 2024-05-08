#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "game_model.hpp"
#include "game_view.hpp"

class GameController {
    public:
        GameController(GameModel& model, GameView& view);
        void handleEvent(const sf::Event& event);
        void handleEvents();
        void handleKeyPress(const sf::Keyboard::Key& code);

    private:
        GameModel& model_;
        GameView& view_;
};

#endif // GAME_CONTROLLER_HPP