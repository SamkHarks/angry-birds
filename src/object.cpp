#include "object.hpp"

Object::Object(b2Body *body) : body_(body) {}

b2Body* Object::getBody() {
    return body_;
}