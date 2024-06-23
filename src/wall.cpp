#include "wall.hpp"
#include "utils.hpp"

Wall::Wall(b2Body *body) : Object(body, Type::Wall, "/assets/box.png") {
    //TODO: Implement this
}

char Wall::getTypeAsChar() const {
    return 'W';
}