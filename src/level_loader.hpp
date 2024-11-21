#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include "bird.hpp"
#include <string>
#include <fstream>
#include "score.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

using json = nlohmann::json;

// Forward declare World class
class World;

struct ShapeData {
    int shapeType; // 0: Circle, 1: Polygon
    b2Vec2 shapePosition; // Position of the shape
    float radius; // Radius of the circle
    float density; // Density of the object
    float friction; // Friction of the object
    float restitution; // Restitution of the 
    b2Vec2 dimensions; // Dimensions of the shape
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

/**
 * @brief Level objects used for creating level
 * 
 * @param sprite The sprite representing the visual appearance of the object
 * @param deleteButton The sprite representing the delete button for the object
 * @param data The data representing the object's properties and used for saving to the level file
 * @param shapeData The shape data representing the object's shape
 * @param isIntersecting Whether the object is intersecting with another object
 * @param intersectingObjects The list of objects that the object is intersecting with
 * @param id The unique ID of the object
 * @param hasDeleteButton Whether the object has a delete button
 */
struct LevelObject {
    int id;
    sf::Sprite sprite;
    sf::Sprite deleteButton;
    ObjectData data;
    ShapeData shapeData;
    bool isIntersecting() const {
        return !intersectingObjects.empty();
    }
    std::unordered_set<int> intersectingObjects = {};
    bool hasDeleteButton = true;
};

class LevelLoader {
    public:
        LevelLoader(World& level);
        void loadLevel(const std::string& fileName);
        void saveHighScores(const std::vector<HighScore> &highScores, const std::string& fileName);
    private:
        World& level_;
        // Helper functions for loading the level
        std::vector<Bird::Type> readBirdList(json levelJson); 
        b2Body* createBody(const ObjectData& data);
        void createFixtureShape(ShapeData data, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes);
        void createObject(Object::Type objType, b2Body* body, b2FixtureDef& fixtureDef, const ShapeData& shapeData);
        void createBird(Bird::Type birdType, b2Body* body, b2FixtureDef& fixtureDef);
        void setLevelName(json levelJson);
        void loadSfmlObjects(std::vector<Bird::Type>& birdList);
};

class LevelCreator {
    public:
        LevelCreator();
        void createLevel(const std::vector<Bird::Type>& birdList, const std::vector<LevelObject>& objects) const;
        void captureScreenShot(const sf::RenderWindow& window) const;
    private:
        json createBirdObject() const;
        json createBirds(const std::vector<Bird::Type>& birdList) const;
        json createObjects(const std::vector<LevelObject>& objects) const;
};

#endif