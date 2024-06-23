#include "bird.hpp"
#include <iostream>
#include "utils.hpp"

Bird::Bird(b2Body *body, std::string texture_file, Bird::Type bird_type, float radius) : 
    Object(body, Object::Type::Bird, texture_file), bird_type_(bird_type) {
        int width = sprite_.getTextureRect().width;
        int height = sprite_.getTextureRect().height;
        sprite_.setScale(radius * SCALE / (static_cast<float>(width)), radius * SCALE / (static_cast<float>(height)));
        sprite_.setOrigin(width / 2, height / 2);
        sf::Vector2 sfPosition = utils::B2ToSfCoords(body_->GetPosition());
        sprite_.setPosition(sfPosition.x, sfPosition.y);
    }

Bird::Type Bird::getBirdType() const {
    return bird_type_;
}

RedBird::RedBird(b2Body *body, float radius) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Red, radius) {}

char RedBird::getTypeAsChar() const {
    return 'R';
}

BlueBird::BlueBird(b2Body *body, float radius) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Blue, radius) {}

char BlueBird::getTypeAsChar() const {
    return 'L';
}

GreenBird::GreenBird(b2Body *body, float radius) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Blue, radius) {}

char GreenBird::getTypeAsChar() const {
    return 'G';
}