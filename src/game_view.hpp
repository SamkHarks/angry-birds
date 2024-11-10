#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <SFML/Graphics.hpp>
#include "game_model.hpp"

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
    private:
        sf::Vector2f defaultCenter_;
        sf::View gameView_;
        bool manualControl_ = true;
        bool updateView_ = false;
};

#endif // GAME_VIEW_HPP