#ifndef WORLD_HPP
#define WORLD_HPP

#include <box2d/box2d.h>
#include <list>
#include "bird.hpp"

const int FRAME_RATE = 60;
const float TIME_STEP = 1.0f / FRAME_RATE;
const int VELOCITY_ITERATIONS = 8;
const int POSITION_ITERATIONS = 3;

struct ObjectData {
    Object::Type type; // Object type
    b2Vec2 position; // Position of the object
    float angle; // Angle of the object
    float angularVelocity; // Angular velocity of the object
    b2Vec2 linearVelocity; // Linear velocity of the object
    float angularDamping; // Angular damping of the object
    float linearDamping; // Linear damping of the object
    float gravityScale; // Gravity scale of the object
    b2BodyType bodyType; // Body type of the object, b2_staticBody, b2_kinematicBody, b2_dynamicBody
    bool awake; // Whether the object is awake

    // Add more fields here as needed...
};

class World {
    public:
        World();
        ~World();
        void loadLevel(const std::string& filename);
        void setLevelName(std::ifstream &file);
        std::vector<Bird::Type> readBirdList(std::ifstream &file); 
        ObjectData readObjectData(std::ifstream &file) const;
        b2Body* createBody(const ObjectData& data);
        void readFixture(std::ifstream& file, b2FixtureDef& fixture_def);
        void createShape(int shape_type, std::stringstream& fixture, b2FixtureDef& fixtureDef);
        void createObject(Object::Type objType, std::vector<Bird::Type> birdList, b2Body* body, b2FixtureDef& fixtureDef);
        void addObject(Object *object);
        void step();
        b2World* getWorld();
        std::list<Object *> getObjects();
    private:
        b2World *world_;
        b2Vec2 gravity_;
        std::list<Object *> objects_;
        std::list<Bird *> birds_;
        std::string name_;
};

#endif // WORLD_HPP