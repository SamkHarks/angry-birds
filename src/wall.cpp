#include "wall.hpp"
#include "utils.hpp"

Wall::Wall(b2Body *body, float hx, float hy) : Object(body, Type::Wall, "/assets/images/box.png", 500, true) {
    float width = static_cast<float>(sprite_.getTextureRect().width);
    float height = static_cast<float>(sprite_.getTextureRect().height);
    float scaleY = (2.f * hy * SCALE) / height;
    float scaleX = (2.f * hx * SCALE) / width;
    sprite_.setScale(scaleX, scaleY);
    sprite_.setOrigin(width / 2.f, height / 2.f);
    sprite_.setPosition(utils::B2ToSfCoords(body->GetPosition()));
    sprite_.setRotation(utils::RadiansToDegrees(body->GetAngle()));

}

char Wall::getTypeAsChar() const {
    return 'W';
}

bool Wall::shouldRemove() const {
    return isDestroyed() || isOutOfBounds();
}

void Wall::handleCollision(Object* objectB) {
    if (!isDestrucable_ || objectB->getType() == Type::Ground) {
        return; // Later add logic for unbreakable walls
    }
    float damage = objectB->getBody()->GetLinearVelocity().LengthSquared();
    switch (objectB->getType()) {
        case Type::Bird:
            damage = damage * 0.25f;
            break;
        case Type::Pig:
            damage = damage * 0.2f;
            break;
        case Type::Wall:
            damage = damage * 0.3f;
            break;
        default:
            break;
    }
    if (damage <= 0.01f) {
        return; // ignore small impacts
    }
    health_ -= damage * objectB->getDamageMultiplier();
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}