#ifndef WORLD_HPP
#define WORLD_HPP

#include <box2d/box2d.h>
#include <list>
#include "bird.hpp"
#include "ground.hpp"
#include "pig.hpp"
#include "wall.hpp"
#include "sfml_debug_draw.hpp"
#include "cannon.hpp"

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

// Define the shapes of the objects
struct Shapes {
    b2CircleShape circle;
    b2PolygonShape polygon;
};

class World {
    public:
        World();
        ~World();
        void loadLevel(const std::string& filename);
        void setLevelName(std::ifstream &file);
        void addObject(Object *object);
        void step();
        void draw(sf::RenderWindow &window) const;
        Bird *GetBird();
        const Bird* GetBird() const;
        b2World* getWorld();
        std::list<Object *> getObjects();
        void setDebugDraw(SFMLDebugDraw* debugDraw);
        void debugDraw() const;
        void removeObject(Object *object);
        void removeBird();
        void resetBird();
        int getRemainingBirdCount() const;
        int getRemainingPigCount() const;
        void setIsSettled(bool isSettled);
        bool getIsSettled() const;
        Cannon* getCannon();
        std::tuple<int,float> getScoreAndStars() const;
    private:
        b2World *world_;
        b2Vec2 gravity_;
        std::list<Object *> objects_;
        std::list<Bird *> birds_;
        std::string name_;
        Cannon *cannon_;
        sf::RectangleShape background_;
        sf::Texture background_image_;
        int totalPigCount_ = 0;
        int totalBirdCount_ = 0;
        bool isSettled_ = false;
        // Helper functions for loading the level
        std::vector<Bird::Type> readBirdList(std::ifstream &file); 
        ObjectData readObjectData(std::ifstream &file) const;
        b2Body* createBody(const ObjectData& data);
        void readFixture(std::ifstream& file, b2FixtureDef& fixture_def, Object::Type &type, Shapes &shapes);
        void createShape(int shape_type, std::stringstream& fixture, b2FixtureDef& fixtureDef, Shapes &shapes, Object::Type &type);
        void createObject(Object::Type objType, std::vector<Bird::Type> birdList, b2Body* body, b2FixtureDef& fixtureDef);
        // Debug draw
        SFMLDebugDraw* debugDraw_ = nullptr;
};

#endif // WORLD_HPP