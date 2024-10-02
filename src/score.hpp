#ifndef SCORE_HPP
#define SCORE_HPP

#include <SFML/Graphics.hpp>
#include "resource_manager.hpp"

class Score {
public:
    Score();
    void update(int score);
    void reset();
    void draw(sf::RenderWindow& window) const;
    void updateHighScore(int highScore);
    void updatePosition(sf::RenderWindow& window);

private:
    int currentScore_ = 0;
    int highScore_ = 0;
    sf::Text text_;
    sf::Font font_;
};

#endif // SCORE_HPP