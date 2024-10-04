#ifndef GAME_OVER_HPP
#define GAME_OVER_HPP

#include "menu.hpp"
#include "score.hpp"

class GameOver: public Menu {
    public:
        GameOver();
        void setScoreManager(Score* score);
        void draw(sf::RenderWindow& window) const override;
    private:
       Score* scoreManager_;
};

#endif // GAME_OVER_HPP
