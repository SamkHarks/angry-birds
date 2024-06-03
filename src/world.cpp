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
    if (object->getType() == Object::Type::Bird) {
        birds_.push_back(dynamic_cast<Bird *>(object));
    } else {
        objects_.push_back(object);
    }
}

void World::loadLevel(const std::string& filename) {
    std::string path = getExecutablePath() + "/assets/levels/";
    std::ifstream file(path + filename);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + filename);
    }
    // Read and set level name from the first line
    setLevelName(file);
    // Read bird list from the second line
    std::vector<Bird::Type> birdList = readBirdList(file);
    // Read and create objects from the rest of the file
    while (!file.eof()) {
        ObjectData data = readObjectData(file);
        b2Body *body = createBody(data);
        b2FixtureDef fixtureDef;
        readFixture(file, fixtureDef);
        createObject(data.type, birdList, body, fixtureDef);
        file.get(); // Read the newline character
    }
}

void World::setLevelName(std::ifstream &file) {
    if (!file.eof()) {
        std::string name;
        std::getline(file, name);
        name_ = name;
    }
}

std::vector<Bird::Type> World::readBirdList(std::ifstream &file) {
    std::vector<Bird::Type> birdList;
    char birdType;
    std::string birdListStr;
    std::getline(file, birdListStr);
    for (auto birdType : birdListStr) {
        switch (birdType) {
            case 'R':
                birdList.push_back(Bird::Type::Red);
                break;
            case 'B':
                birdList.push_back(Bird::Type::Blue);
                break;
            case 'G':
                birdList.push_back(Bird::Type::Green);
                break;
            default:
                throw std::runtime_error("Invalid bird type");
        }
    }
    return birdList;
}

ObjectData World::readObjectData(std::ifstream &file) const {
    char _; // Character to ignore
    char type;
    file.get(type);
    file.ignore();
    std::cout<<type<<std::endl;
    ObjectData data;
    // Read the object type from the file
    switch (type) {
        case 'B':
            data.type = Object::Type::Bird;
            break;
        case 'G':
            data.type = Object::Type::Ground;
            break;
        case 'P':
            data.type = Object::Type::Pig;
            break;
        case 'W':
            data.type = Object::Type::Wall;
            break;
        default:
            throw std::runtime_error("Invalid object type");
            break;
    }

    // Read ObjectData fields from the file
    file>>data.position>>_
    >>data.angle>>_
    >>data.angularVelocity>>_
    >>data.linearVelocity>>_
    >>data.angularDamping>>_
    >>data.linearDamping>>_
    >>data.gravityScale>>_
    >>data.bodyType>>_
    >>data.awake>>_;
    
    return data;
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
    body_def.type = data.bodyType;
    body_def.awake = data.awake;

    b2Body *body = world_->CreateBody(&body_def);

    return body;
}

void World::createShape(int shapeType, std::stringstream& fixture, b2FixtureDef& fixtureDef) {
    char _; // Character to ignore
    switch (shapeType) {
        case b2Shape::Type::e_circle: {
            b2CircleShape* circle = new b2CircleShape;
            fixture >> circle->m_p >> _ >> circle->m_radius >> _;
            fixtureDef.shape = circle;
            break;
        }
        case b2Shape::Type::e_polygon:
            //TODO: Implement polygon shape
            break;
        default:
            throw std::runtime_error("Unknown shape type");
    }
}

void World::readFixture(std::ifstream& file, b2FixtureDef& fixtureDef) {
    char _; // Character to ignore
    std::string fixtureStr;
    std::getline(file, fixtureStr, '\n');
    std::stringstream fixture(fixtureStr);
    int shapeType;
    fixture >> shapeType >> _;
    createShape(shapeType, fixture, fixtureDef);
    fixture >> fixtureDef.density >> _ >> fixtureDef.friction >> _ >> fixtureDef.restitution >> _;
}

void World::createObject(
    Object::Type objType,
    std::vector<Bird::Type> birdList,
    b2Body* body,
    b2FixtureDef& fixtureDef
) {
    //Object *object = nullptr;
    switch (objType) {
        case Object::Type::Bird:
            for (auto birdType : birdList) {
                Bird *bird;
                switch (birdType) {
                    case Bird::Type::Red:
                        bird = new RedBird(body);
                        break;
                    case Bird::Type::Blue:
                        // TODO: Implement Yellow bird
                        break;
                    case Bird::Type::Green:
                        // TODO: Implement Green bird
                        break;
                }
                addObject(bird);
                fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(bird);
            }
            break;
        case Object::Type::Ground:
            //TODO: Implement Ground object
            break;
        case Object::Type::Pig:
            //TODO: Implement Pig object
            break;
        case Object::Type::Wall:
            //TODO: Implement Wall object
            break;
        default:
            throw std::runtime_error("Invalid object type");
    }
}


void World::step() {
    world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

b2World* World::getWorld() {
    return world_;
}

std::list<Object *> World::getObjects() {
    return objects_;
}