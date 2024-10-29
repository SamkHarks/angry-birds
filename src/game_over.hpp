#ifndef GAME_OVER_HPP
#define GAME_OVER_HPP

#include "menu.hpp"
#include "score.hpp"

class GameOver: public Menu {
    public:
        GameOver();
        void setScoreManager(Score* score);
        void draw(sf::RenderWindow& window) const override;
        void drawStars(int stars, sf::RenderWindow& window) const;
    private:
        Score* scoreManager_;
        std::vector<sf::Texture> starTextures_;
        std::vector<sf::Sprite> starSprites_;
        sf::Clock clock_;
};

#endif // GAME_OVER_HPP
