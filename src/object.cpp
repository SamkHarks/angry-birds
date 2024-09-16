#include "object.hpp"
#include "utils.hpp"

Object::Object(b2Body *body, Type type, std::string texture_file, float health, bool isDestrucable) : body_(body), type_(type), health_(health), isDestrucable_(isDestrucable) {
    if (!utils::loadFromFile(texture_, texture_file)) {
        throw std::runtime_error("Failed to load texture file: " + texture_file);
    }
    sprite_.setTexture(texture_);
}

b2Body* Object::getBody() {
    return body_;
}

const b2Body* Object::getBody() const {
    return body_;
}

Object::Type Object::getType() const {
    return type_;
}


const sf::Sprite& Object::getSprite() const {
    return sprite_;
}
sf::Sprite& Object::getSprite() {
    return sprite_;
}

void Object::handelCollision(float damage) {
    if (!isDestrucable_ || damage <= 0.01f) {
        return;
    }
    health_ -= damage;
    if (health_ <= 0) {
        body_->GetWorld()->DestroyBody(body_);
        isDestroyed_ = true;
    }
}

bool Object::isDestroyed() const {
    return isDestroyed_;
}