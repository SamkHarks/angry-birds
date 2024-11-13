#include "ground.hpp"
#include "utils.hpp"


Ground::Ground(b2Body *body, float hx, float hy) : Object(body, Type::Ground, "/assets/images/ground.png") {
    float width = static_cast<float>(sprite_.getTextureRect().width);
    float height = static_cast<float>(sprite_.getTextureRect().height);
    float heightSf = utils::B2ToSf(2.f * hy);
    float scaleX = utils::B2ToSf(2.f * hx) / width;
    float scaleY = heightSf / height;
    float scaleFactor = utils::getScaleFactor(width, height, utils::B2ToSf(2.f * hx), heightSf);
    sprite_.setScale(scaleFactor, scaleFactor);
    sf::Vector2f centerPosition = utils::B2ToSfCoords(body->GetPosition());
    sprite_.setPosition(0, centerPosition.y - heightSf);
}

char Ground::getTypeAsChar() const {
    return 'G';
}

void Ground::update() {
    b2Vec2 position = body_->GetPosition();
    sf::Sprite& sprite = this->getSprite();
    sf::Vector2f centerPosition = utils::B2ToSfCoords(position);
    float heightSf = 100.f;
    sprite_.setPosition(0, centerPosition.y - heightSf);
}

bool Ground::isMoving() const {
    return false;
}