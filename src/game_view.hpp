#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <SFML/Graphics.hpp>
#include "game_model.hpp"
#include "event.hpp"

class GameView: public sf::RenderWindow {
    public:
        GameView();
        void draw(const GameModel& model);
        void render(const GameModel& model);
        void updateCamera(GameModel& model);
        void updateCamera(const sf::Keyboard::Key& code);
        void updateHUD(GameModel& model);
        void setGameView();
        void setGameView(const sf::View& view);
        void handleResize(const float& width, const float& height);
        void setEventDispatcher(EventDispatcher* eventDispatcher);
    private:
        sf::Vector2f defaultCenter_;
        sf::View gameView_;
        EventDispatcher* eventDispatcher_ = nullptr;
        bool manualControl_ = true;
        bool updateView_ = false;
};

#endif // GAME_VIEW_HPP