#include "bird.hpp"
#include <iostream>
#include "utils.hpp"

Bird::Bird(b2Body *body, std::string texture_file, Bird::Type bird_type, float radius) : 
    Object(body, Object::Type::Bird, texture_file, 100.f, true), bird_type_(bird_type) {
        int width = sprite_.getTextureRect().width;
        int height = sprite_.getTextureRect().height;
        sprite_.setScale(radius * SCALE * 2.f / (1.f * width), radius * SCALE * 2.f / (1.f * height));
        sprite_.setOrigin(width / 2.f, height / 2.f);
    }

Bird::Type Bird::getBirdType() const {
    return bird_type_;
}

bool Bird::isLaunched() const {
    return is_launched_;
}

void Bird::setLaunched(bool launched) {
    pressClock_.restart();
    is_launched_ = launched;
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
    if (!is_launched_) { 
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