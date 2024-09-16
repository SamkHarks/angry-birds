#include "ground.hpp"
#include "utils.hpp"


Ground::Ground(b2Body *body, float hx, float hy) : Object(body, Type::Ground, "/assets/images/ground.png") {
    float width = static_cast<float>(sprite_.getTextureRect().width);
    float height = static_cast<float>(sprite_.getTextureRect().height);
    float heightY = 2.f * hy * SCALE;
    float scaleX = (2.f * hx * SCALE) / width;
    float scaleY = (heightY + 30) / height;
    sprite_.setScale(scaleX, scaleY);
    texture_.setRepeated(true);
    sf::Vector2f centerPosition = utils::B2ToSfCoords(body->GetPosition());
    sprite_.setPosition(0, centerPosition.y - (heightY - 20));
}

char Ground::getTypeAsChar() const {
    return 'G';
}