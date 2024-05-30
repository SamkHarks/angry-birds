#include "object.hpp"
#include "utils.hpp"

Object::Object(b2Body *body, Type type, std::string texture_file) : body_(body), type_(type) {
    if (!loadFromFile(texture_, texture_file)) {
        throw std::runtime_error("Failed to load texture file: " + texture_file);
    }
    sprite_.setTexture(texture_);
}

b2Body* Object::getBody() {
    return body_;
}

Object::Type Object::getType() const {
    return type_;
}

sf::Sprite& Object::getSprite() {
    return sprite_;
}