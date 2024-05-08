#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <SFML/Graphics.hpp>

class GameView {
    public:
        GameView();
        void draw();
        void render();
        sf::RenderWindow& getWindow();
    private:
        sf::RenderWindow window_;
};

#endif // GAME_VIEW_HPP