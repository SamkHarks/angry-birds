#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <SFML/Graphics.hpp>
#include "game_model.hpp"

class GameView: public sf::RenderWindow {
    public:
        GameView();
        void draw(const GameModel& model);
        void render(const GameModel& model);
};

#endif // GAME_VIEW_HPP