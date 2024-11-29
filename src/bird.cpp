#include "bird.hpp"
#include <iostream>
#include "utils.hpp"

Bird::Bird(b2Body *body, std::string textureFile, Bird::Type birdType, float radius) : 
    Object(body, Object::Type::Bird, textureFile, 100.f, true), birdType_(birdType) {
        int width = sprite_.getTextureRect().width;
        int height = sprite_.getTextureRect().height;
        sprite_.setScale(radius * SCALE * 2.f / (1.f * width), radius * SCALE * 2.f / (1.f * height));
        sprite_.setOrigin(width / 2.f, height / 2.f);
    }

Bird::Type Bird::getBirdType() const {
    return birdType_;
}

bool Bird::isLaunched() const {
    return isLaunched_;
}

void Bird::setLaunched(bool launched) {
    pressClock_.restart();
    isLaunched_ = launched;
}

RedBird::RedBird(b2Body *body, float radius) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Red, radius) {}

char RedBird::getTypeAsChar() const {
    return 'R';
}

BlueBird::BlueBird(b2Body *body, float radius) : Bird(body, "/assets/images/blue_bird.png", Bird::Type::Blue, radius) {}

char BlueBird::getTypeAsChar() const {
    return 'L';
}

GreenBird::GreenBird(b2Body *body, float radius) : Bird(body, "/assets/images/green_bird.png", Bird::Type::Green, radius) {}

char GreenBird::getTypeAsChar() const {
    return 'G';
}

bool Bird::shouldRemove() const {
    if (isDestroyed()) {
        return true;
    }
    if (!isLaunched_) { 
        return false;
    }
   	if (this->isOutOfBounds()) {
		return true;
	}
    if (getActiveTime() >= 7.f) {
        return true;
    }
    return false;
}

void Bird::setIsPaused(bool paused) {
    if (paused) {
        pauseClock_.restart();
    } else {
        pausedTime_ += pauseClock_.getElapsedTime().asSeconds();
    }
    isPaused_ = paused;
}

float Bird::getActiveTime() const {
    return pressClock_.getElapsedTime().asSeconds() - pausedTime_;
}

bool Bird::getIsPaused() const {
    return isPaused_;
}

void RedBird::usePower() {
    if (isPowerUsed_ || !isLaunched_) {
        return;
    }
    float power = 10.f;
    b2Vec2 force = b2Vec2(0, -power); // Apply force downwards
    body_->ApplyLinearImpulse(force, body_->GetPosition(), true);
    body_->SetBullet(true);
    isPowerUsed_ = true;
}

void BlueBird::usePower() {
    if (isPowerUsed_) {
        return;
    }
    //TODO: Implement power
}

void GreenBird::usePower() {
    if (isPowerUsed_) {
        return;
    }
    isPowerUsed_ = true;
}

void GreenBird::handleKeyPress(const sf::Keyboard::Key& code) {
    if (!isPowerUsed_) {
        return; // Power not yet activated
    }
    float controlForce = 20.0f;
    b2Vec2 force(0.0f, 0.0f);
    auto maxSpeed = 5.0f; // Maximum speed of the bird
    // Check for the key pressed and modify the force vector accordingly
    switch (code) {
        case sf::Keyboard::W:
            force.y = controlForce;
            break;
        case sf::Keyboard::S:
            force.y = -controlForce;
            break;
        case sf::Keyboard::A:
            force.x = -controlForce;
            break;
        case sf::Keyboard::D:
            force.x = controlForce;
            break;
    }

    // Apply the force to the bird's body at its center of mass to modify direction
    body_->ApplyForceToCenter(force, true);

    // Limit the speed of the bird
    b2Vec2 velocity = body_->GetLinearVelocity();
    float currentSpeed = velocity.Length();

    // Clamp the speed to the maximum value
    if (currentSpeed > maxSpeed) {
        velocity *= (maxSpeed / currentSpeed); // Normalize and scale down the velocity
        body_->SetLinearVelocity(velocity);
    }
}

void Bird::handleCollision(Object* objectB) {
    float damage = objectB->getBody()->GetLinearVelocity().LengthSquared();
    switch (objectB->getType()) {
        case Object::Type::Pig:
            damage = damage * 0.5;
            break;
        case Object::Type::Wall:
            damage = damage * 1.5f;
            break;
        case Object::Type::Ground:
            if (prevY_ > 0.95f) {
                damage = body_->GetLinearVelocity().Length() * 0.1f;
            } else {
                damage = 0;
            }
            break;
        default:
            break;
    }
    
    if (damage <= 0.1f) {
        return;
    }
    health_ -= damage * this->getDamageMultiplier();
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}

bool Bird::getIsPowerUsed() const {
    return isPowerUsed_;
}

int Bird::getDamageMultiplier() const {
    return 1;
}

int GreenBird::getDamageMultiplier() const {
    if (isPowerUsed_) {
        return 10;
    } else {
        return 1;
    }
}