#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include "bird.hpp"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward declare World class
class World;

struct ShapeData {
    int shapeType; // 0: Circle, 1: Polygon
    b2Vec2 shapePosition; // Position of the shape
    float radius; // Radius of the circle
    float density; // Density of the object
    float friction; // Friction of the object
    float restitution; // Restitution of the object
};

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
};

// Define the shapes of the objects
struct Shapes {
    b2CircleShape circle;
    b2PolygonShape polygon;
};

class LevelLoader {
    public:
        LevelLoader(World& level);
        void loadLevel(const std::string& fileName);
        void saveHighScore(int score, const std::string& fileName);
    private:
        World& level_;
        // Helper functions for loading the level
        std::vector<Bird::Type> readBirdList(json levelJson); 
        b2Body* createBody(const ObjectData& data);
        void createFixtureShape(ShapeData data, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes);
        void createObject(Object::Type objType, b2Body* body, b2FixtureDef& fixtureDef);
        void createBird(Bird::Type birdType, b2Body* body, b2FixtureDef& fixtureDef);
        void setLevelName(json levelJson);
};

#endif