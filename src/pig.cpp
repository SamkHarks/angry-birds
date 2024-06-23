#include "pig.hpp"
#include "utils.hpp"

Pig::Pig(b2Body *body, float radius) : Object(body, Object::Type::Pig, "/assets/images/pig.png") {
    int width = sprite_.getTextureRect().width;
    int height = sprite_.getTextureRect().height;
    sprite_.setScale(radius * SCALE / (static_cast<float>(width)), radius * SCALE / (static_cast<float>(height)));
    sprite_.setOrigin(width / 2, height / 2);
    sf::Vector2 sfPosition = utils::B2ToSfCoords(body_->GetPosition());
    sprite_.setPosition(sfPosition.x, sfPosition.y);
}

char Pig::getTypeAsChar() const {
    return 'P';
}