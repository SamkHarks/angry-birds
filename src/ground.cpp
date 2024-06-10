#include "ground.hpp"
#include "utils.hpp"


Ground::Ground(b2Body *body) : Object(body, Type::Ground, "/assets/images/ground.jpg") {
    float width = static_cast<float>(sprite_.getTextureRect().width);
    float height = static_cast<float>(sprite_.getTextureRect().height);
    // Set the scale of the sprite
    sprite_.setScale(5.1f * SCALE/width, 5.0f * SCALE/height);

    texture_.setRepeated(true);

    // Set the texture rectangle
    sprite_.setTextureRect({0, 0, static_cast<int>(5.1f * SCALE), static_cast<int>(0.5f * SCALE)});

    // Set the origin of the sprite to its bottom-left corner
    sprite_.setOrigin(0, -290);
}

char Ground::getTypeAsChar() const {
    return 'G';
}