#include "pig.hpp"
#include "utils.hpp"

Pig::Pig(b2Body *body, float radius) : Object(body, Object::Type::Pig, "/assets/images/pig.png", 100, true) {
    int width = sprite_.getTextureRect().width;
    int height = sprite_.getTextureRect().height;

    sprite_.setScale(radius * SCALE * 2.f / (1.f * width), radius * SCALE * 2.f / (1.f * height));
    sprite_.setOrigin(width / 2.f, height / 2.f);
}

char Pig::getTypeAsChar() const {
    return 'P';
}