#include "object.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

Object::Object(b2Body *body, Type type, std::string texture_file, float health, bool isDestrucable) : body_(body), type_(type), health_(health), isDestrucable_(isDestrucable) {
    texture_ = ResourceManager::getInstance().getTexture(texture_file);
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

void Object::handleCollision(float damage) {
    if (!isDestrucable_ || damage <= 0.01f) {
        return;
    }
    health_ -= damage;
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}

bool Object::isDestroyed() const {
    return isDestroyed_;
}

bool Object::isOutOfBounds() const {
    sf::Vector2f position = utils::B2ToSfCoords(body_->GetPosition());
    return position.x < 0 || position.y > VIEW_HEIGHT || position.x > VIEW_WIDTH * 2;
}

void Object::update() {
    b2Vec2 position = body_->GetPosition();
    sf::Sprite& sprite = this->getSprite();
    sf::Vector2f position_pixels = utils::B2ToSfCoords(position);
    sprite.setPosition(position_pixels.x, position_pixels.y);
    float radians = body_->GetAngle();
    float deg = utils::RadiansToDegrees(radians);
    sprite.setRotation(deg);
}

bool Object::isMoving() const {
    return (
        body_->GetLinearVelocity().LengthSquared() > IS_SETTLED_THRESHOLD
        || fabs(body_->GetAngularVelocity()) > IS_SETTLED_THRESHOLD
    );
}

int Object::getDestructionScore() const {
    return 0;
}