#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

class Game {
    public:
        Game();
        void run();

    private:
        sf::RenderWindow _window;
        void render();
        void update();
        void handleEvents();
};

#endif // GAME_HPP