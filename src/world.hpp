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
#include "level_loader.hpp"

/*
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

    // Add more fields here as needed...
};*/
/*
// Define the shapes of the objects
struct Shapes {
    b2CircleShape circle;
    b2PolygonShape polygon;
};*/

class World {
    public:
        World();
        ~World();
        void loadLevel(const std::string& filename);
        void setLevelName(json levelJson);
        void resetLevel();
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
        std::string levelName_;
        Cannon *cannon_;
        sf::RectangleShape background_;
        sf::Texture background_image_;
        int totalPigCount_ = 0;
        int totalBirdCount_ = 0;
        bool isSettled_ = false;
        std::string fileName_;
        // Helper functions for loading the level
        /*std::vector<Bird::Type> readBirdList(json levelJson); 
        b2Body* createBody(const ObjectData& data);
        void createFixtureShape(ShapeData data, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes);
        void createObject(Object::Type objType, std::vector<Bird::Type> birdList, b2Body* body, b2FixtureDef& fixtureDef);
        */
        // Debug draw
        SFMLDebugDraw* debugDraw_ = nullptr;

        LevelLoader levelLoader_;
        
        friend class LevelLoader;
};

#endif // WORLD_HPP