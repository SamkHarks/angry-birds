#include "wall.hpp"
#include "utils.hpp"

Wall::Wall(b2Body *body) : Object(body, Type::Wall, "/assets/box.png") {
    int width = sprite_.getTextureRect().width;
    int height = sprite_.getTextureRect().height;
    sprite_.setScale(1.0f * SCALE / (static_cast<float>(width)), 1.0f * SCALE / (static_cast<float>(height)));
    sprite_.setOrigin(width / 2, height / 2);
    sf::Vector2 sfPosition = utils::B2ToSfCoords(body_->GetPosition());
    sprite_.setPosition(sfPosition.x, sfPosition.y);
}