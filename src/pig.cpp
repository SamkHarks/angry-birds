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

int Pig::getDestructionScore() const {
    return PIG_DESTRUCTION_SCORE;
}

bool Pig::shouldRemove() const {
    return isDestroyed() || isOutOfBounds();
}

void Pig::handleCollision(Object* objectB) {
    float damage = objectB->getBody()->GetLinearVelocity().LengthSquared();
    switch (objectB->getType()) {
        case Object::Type::Bird:
            damage = damage * 0.1f;
            break;
        case Object::Type::Wall:
            damage = damage * 1.1f;
            break;
        case Object::Type::Ground:
            damage = body_->GetLinearVelocity().LengthSquared() * 0.1f;
            break;
        default:
            break;
    }
    if (damage <= 0.5f) {
        return; // ignore small impacts
    }
    health_ -= damage;
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}