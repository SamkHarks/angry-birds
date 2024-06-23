#include "pig.hpp"

Pig::Pig(b2Body *body) : Object(body, Object::Type::Pig, "/assets/images/red_bird.png") {}

char Pig::getTypeAsChar() const {
    return 'P';
}