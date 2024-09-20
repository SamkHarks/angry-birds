#ifndef CANNON_HPP
#define CANNON_HPP

#include <SFML/Graphics.hpp>
#include "bird.hpp"

class Cannon {
    public:
        Cannon(std::string barrel_texture_file, std::string wheels_texture_file, std::string font_file);
        void setAngle(float angle);
        void draw(sf::RenderWindow &window) const;
        void setPower(float duration);
        void launchBird(Bird* bird);
        void startLaunch();
        void update();
        bool isLaunching() const;

    private:
        sf::Texture barrel_texture_;
        sf::Texture wheels_texture_;
        sf::Sprite wheels_sprite_;
        sf::Sprite barrel_sprite_;
        sf::Text power_text_;
        sf::Font font_; 
        sf::Clock pressClock_;
        float power_ = 0;
        bool isLaunching_ = false;
};

#endif // CANNON_HPP