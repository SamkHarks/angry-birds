#ifndef WORLD_HPP
#define WORLD_HPP

#include <box2d/box2d.h>
#include <list>
#include "bird.hpp"

struct ObjectData {
    Object::Type type; // Object type
    b2Vec2 position; // Position of the object
    float angle; // Angle of the object
    float angularVelocity; // Angular velocity of the object
    b2Vec2 linearVelocity; // Linear velocity of the object
    float angularDamping; // Angular damping of the object
    float linearDamping; // Linear damping of the object
    float gravityScale; // Gravity scale of the object
    int bodyType; // Body type of the object, b2_staticBody, b2_kinematicBody, b2_dynamicBody
    bool awake; // Whether the object is awake

    // Add more fields here as needed...
};

class World {
    public:
        World();
        ~World();
        void loadLevel(const std::string& filename);
        void setLevelName(std::ifstream &file);
        ObjectData readObjectData(std::ifstream &file) const;
        b2Body* createBody(const ObjectData& data); 
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