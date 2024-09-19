#ifndef CANNON_HPP
#define CANNON_HPP

#include <SFML/Graphics.hpp>
#include "bird.hpp"

class Cannon {
    public:
        Cannon(std::string barrel_texture_file, std::string wheels_texture_file, std::string font_file);
        void setAngle(float angle);
        void draw(sf::RenderWindow &window) const;
        void setPower(sf::Vector2f difference);
        void launchBird(Bird* bird);

    private:
        sf::Texture barrel_texture_;
        sf::Texture wheels_texture_;
        sf::Sprite wheels_sprite_;
        sf::Sprite barrel_sprite_;
        sf::Vector2f difference_ = sf::Vector2f(0, 0);
        sf::Text power_text_;
        sf::Font font_;
        float power_ = 0;
};

#endif // CANNON_HPP