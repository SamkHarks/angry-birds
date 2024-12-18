#include "object.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

Object::Object(b2Body *body, Type type, std::string texture_file, float health, bool isDestrucable) : body_(body), type_(type), health_(health), isDestrucable_(isDestrucable) {
    sprite_.setTexture(ResourceManager::getInstance().getTexture(texture_file));
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

void Object::handleCollision(Object* objectB) {
    // Default implementation does nothing
}

bool Object::isDestroyed() const {
    return isDestroyed_;
}

bool Object::isOutOfBounds() const {
    sf::Vector2f position = utils::B2ToSfCoords(body_->GetPosition());
    return position.x < 0 || position.y > VIEW.getHeight() || position.x > WORLD_WIDTH;
}

void Object::update() {
    b2Vec2 position = body_->GetPosition();
    prevY_ = position.y;
    sf::Sprite& sprite = this->getSprite();
    sf::Vector2f position_pixels = utils::B2ToSfCoords(position);
    sprite.setPosition(position_pixels.x, position_pixels.y);
    float radians = body_->GetAngle();
    float deg = utils::RadiansToDegrees(radians);
    sprite.setRotation(-deg);
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

bool Object::shouldRemove() const {
    return false;
}

void Object::draw(sf::RenderWindow &window) const {
    window.draw(sprite_);
}