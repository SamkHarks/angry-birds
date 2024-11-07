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
    float duration = pressClock_.getElapsedTime().asSeconds();
    if (duration >= 5.f) {
        return true;
    }
    return false;
}