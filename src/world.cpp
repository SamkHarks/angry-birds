#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

World::World() : gravity_(0.0f, 9.8f) {
    world_ = new b2World(gravity_);
}

World::~World() {
    delete world_;
}

void World::addObject(Object *object) {
    objects_.push_back(object);
}

void World::loadLevel(const std::string& filename) {
    std::string path = getExecutablePath() + "/assets/levels/";
    std::ifstream file(path + filename);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + filename);
    }
    setLevelName(file);
}

void World::setLevelName(std::ifstream &file) {
    if (!file.eof()) {
        std::string name;
        std::getline(file, name);
        name_ = name;
    }
}

ObjectData World::readObjectData(std::ifstream &file) const {
    char type;
    file.get(type);
    file.ignore();
    std::cout<<type<<std::endl;
    ObjectData data;
    return data; // Remove this line and add your implementation here
}

b2Body* World::createBody(const ObjectData& data) {
    b2BodyDef body_def;
    body_def.position = data.position;
    body_def.angle = data.angle;
    body_def.angularVelocity = data.angularVelocity;
    body_def.linearVelocity = data.linearVelocity;
    body_def.angularDamping = data.angularDamping;
    body_def.linearDamping = data.linearDamping;
    body_def.gravityScale = data.gravityScale;
    body_def.type = static_cast<b2BodyType>(data.bodyType);
    body_def.awake = data.awake;

    b2Body *body = world_->CreateBody(&body_def);

    return body;
}

void World::step() {
    world_->Step(1.0f / 60.0f, 8, 3);
}

b2World* World::getWorld() {
    return world_;
}

std::list<Object *> World::getObjects() {
    return objects_;
}