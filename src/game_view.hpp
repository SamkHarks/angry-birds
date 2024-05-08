#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <SFML/Graphics.hpp>
#include "game_model.hpp"

class GameView: public sf::RenderWindow {
    public:
        GameView();
        void draw(const GameModel::State& state);
        void render(const GameModel::State& state);
};

#endif // GAME_VIEW_HPP