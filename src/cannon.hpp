#ifndef CANNON_HPP
#define CANNON_HPP

#include <SFML/Graphics.hpp>
#include "bird.hpp"

class Cannon {
    public:
        Cannon();
        void setAngle(float angle);
        void draw(sf::RenderWindow &window) const;
        void setPower(float duration);
        void launchBird(Bird* bird);
        void startLaunch();
        void update();
        bool isLaunching() const;
        void reset();
        void updateTextPosition(sf::RenderWindow& window);
        int getTextWidth() const;

    private:
        sf::Texture barrelTexture_;
        sf::Texture wheelsTexture_;
        sf::Sprite wheelsSprite_;
        sf::Sprite barrelSprite_;
        sf::Text powerText_;
        sf::Font font_; 
        sf::Clock pressClock_;
        float power_ = 0;
        bool isLaunching_ = false;
};

#endif // CANNON_HPP