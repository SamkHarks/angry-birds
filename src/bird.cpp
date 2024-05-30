#include "bird.hpp"

Bird::Bird(b2Body *body, std::string texture_file, Bird::Type bird_type) : 
    Object(body, Object::Type::Bird, texture_file), bird_type_(bird_type) {
        int width = sprite_.getTextureRect().width;
        int height = sprite_.getTextureRect().height;
        sprite_.setScale(0.1, 0.1);
        sprite_.setOrigin(width / 2, height / 2);
        sprite_.setPosition(200,200);
    }

Bird::Type Bird::getBirdType() const {
    return bird_type_;
}

RedBird::RedBird(b2Body *body) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Red) {}

char RedBird::getTypeAsChar() const {
    return 'R';
}