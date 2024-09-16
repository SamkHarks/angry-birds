#include "ground.hpp"
#include "utils.hpp"


Ground::Ground(b2Body *body, float hx, float hy) : Object(body, Type::Ground, "/assets/images/ground.png") {
    float width = static_cast<float>(sprite_.getTextureRect().width);
    float height = static_cast<float>(sprite_.getTextureRect().height);
    float heightSf = utils::B2ToSf(2.f * hy);
    float scaleX = utils::B2ToSf(2.f * hx) / width;
    float scaleY = heightSf / height;
    sprite_.setScale(scaleX, scaleY);
    texture_.setRepeated(true);
    sf::Vector2f centerPosition = utils::B2ToSfCoords(body->GetPosition());
    sprite_.setPosition(0, centerPosition.y - heightSf);
}

char Ground::getTypeAsChar() const {
    return 'G';
}