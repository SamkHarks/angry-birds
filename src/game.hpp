#ifndef GAME_HPP
#define GAME_HPP

#include "game_model.hpp"
#include "game_view.hpp"
#include "game_controller.hpp"
#include "event.hpp"

class Game {
    public:
        Game(); 
        void run();

    private:
        GameModel model_;
        GameView view_;
        GameController controller_;
        EventQueue eventQueue_;
        std::unique_ptr<EventDispatcher> eventDispatcher_;
        float elapsedTime_ = 0;
        sf::Clock timer;
        void controlFrameRate();
};

#endif // GAME_HPP