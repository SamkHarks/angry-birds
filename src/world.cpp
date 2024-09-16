#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

World::World() : gravity_(0.0f, -9.8f) {
    world_ = new b2World(gravity_);
}

World::~World() {
    for (auto object : objects_) {
        delete object;
    }
    for (auto bird : birds_) {
        delete bird;
    }
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
    std::string path = utils::getExecutablePath() + "/assets/levels/";
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
        Shapes shapes;
        readFixture(file, fixtureDef, data.type, shapes);
        createObject(data.type, birdList, body, fixtureDef);
        body->CreateFixture(&fixtureDef);
        //file.get(); // Read the newline character
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
            case 'L':
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
    switch (data.type) {
        case Object::Type::Bird:
        case Object::Type::Pig:
        case Object::Type::Wall:
            file>>data.position>>_
                >>data.angle>>_
                >>data.angularVelocity>>_
                >>data.linearVelocity>>_
                >>data.angularDamping>>_
                >>data.linearDamping>>_
                >>data.gravityScale>>_
                >>data.bodyType>>_
                >>data.awake>>_;
            break;
        case Object::Type::Ground:
            data.position = b2Vec2(0, 0);
            data.bodyType = b2BodyType::b2_staticBody;
            break;
        default:
            throw std::runtime_error("Invalid object type");
    }
    return data;
}

b2Body* World::createBody(const ObjectData& data) {
    b2BodyDef body_def;
    switch (data.type) {
        case Object::Type::Bird:
        case Object::Type::Pig:
        case Object::Type::Wall:
            body_def.position = data.position;
            body_def.angle = data.angle;
            body_def.angularVelocity = data.angularVelocity;
            body_def.linearVelocity = data.linearVelocity;
            body_def.angularDamping = data.angularDamping;
            body_def.linearDamping = data.linearDamping;
            body_def.gravityScale = data.gravityScale;
            body_def.type = data.bodyType;
            body_def.awake = data.awake;
            break;
        case Object::Type::Ground:
            body_def.position = data.position;
            body_def.type = data.bodyType;
    }

    b2Body *body = world_->CreateBody(&body_def);

    return body;
}

void World::createShape(int shapeType, std::stringstream& fixture, b2FixtureDef& fixtureDef, Shapes &shapes, Object::Type &type) {
    char _; // Character to ignore
    switch (shapeType) {
        case b2Shape::Type::e_circle: {
            fixture >> shapes.circle.m_p >> _ >> shapes.circle.m_radius >> _;
            fixtureDef.shape = &shapes.circle;
            break;
        }
        case b2Shape::Type::e_polygon:
            //TODO: Implement polygon shape
             if (type == Object::Type::Ground) {
                // Define the ground shape
                shapes.polygon.SetAsBox(5.1f, 0.5f);
                fixtureDef.shape = &shapes.polygon;
             }
            break;
        default:
            throw std::runtime_error("Unknown shape type");
    }
}

void World::readFixture(std::ifstream& file, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes) {
    char _; // Character to ignore
    std::string fixtureStr;
    std::getline(file, fixtureStr, '\n');
    std::stringstream fixture(fixtureStr);
    // Read the shape type and create the shape
    int shapeType;
    fixture >> shapeType >> _;
    createShape(shapeType, fixture, fixtureDef, shapes, type);
    // Read the rest of the fixture data, density, friction, restitution
    if (type == Object::Type::Ground) {
        fixtureDef.friction = 0.2f;
    } else {
        fixture >> fixtureDef.density >> _
                >> fixtureDef.friction >> _
                >> fixtureDef.restitution >> _;
    }
}

void World::createObject(
    Object::Type objType,
    std::vector<Bird::Type> birdList,
    b2Body* body,
    b2FixtureDef& fixtureDef
) {
    Object *object = nullptr;
    switch (objType) {
        case Object::Type::Bird:
            for (auto birdType : birdList) {
                Bird *bird;
                switch (birdType) {
                    case Bird::Type::Red:
                        bird = new RedBird(body, fixtureDef.shape->m_radius);                       
                        break;
                    case Bird::Type::Blue:
                        bird = new BlueBird(body, fixtureDef.shape->m_radius);
                        break;
                    case Bird::Type::Green:
                        bird = new GreenBird(body, fixtureDef.shape->m_radius);
                        break;
                }
                addObject(bird);
                fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(bird);
            }
            break;
        case Object::Type::Ground:
            object = new Ground(body);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Pig:
            object = new Pig(body, fixtureDef.shape->m_radius);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Wall:
            object = new Wall(body);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        default:
            throw std::runtime_error("Invalid object type");
    }
    if (object) {
        addObject(object);
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

Bird* World::GetBird() {
    return birds_.front();
}

const Bird* World::GetBird() const {
    return birds_.front();
}

void World::draw(sf::RenderWindow &window) const {
    for (auto object : objects_) {
        window.draw(object->getSprite());
    }
    const Bird* bird = GetBird();
    const sf::Sprite& sprite = bird->getSprite();
    window.draw(sprite);
}

void World::debugDraw() const {
    // Loop through all objects and draw their Box2D shapes
    sf::Color sfmlRed = sf::Color::Red;
    sf::Color sfmlGreen = sf::Color::Green;
    b2Color box2DRed(sfmlRed.r / 255.0f, sfmlRed.g / 255.0f, sfmlRed.b / 255.0f);
    b2Color box2DGreen(sfmlGreen.r / 255.0f, sfmlGreen.g / 255.0f, sfmlGreen.b / 255.0f);
    for (auto object : objects_) {
        auto body = object->getBody(); // Assuming this method exists and returns a b2Body*
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            
            // Depending on the type of the fixture, call the appropriate SFMLDebugDraw method
            if (f->GetType() == b2Shape::e_polygon) {
                // Cast the shape to a polygon shape to access vertices
                b2PolygonShape* polygonShape = static_cast<b2PolygonShape*>(f->GetShape());
                debugDraw_->DrawPolygon(polygonShape->m_vertices, polygonShape->m_count, box2DRed);
            } else if (f->GetType() == b2Shape::e_circle) {
                // Cast the shape to a circle shape to access center and radius
                b2CircleShape* circleShape = static_cast<b2CircleShape*>(f->GetShape());
                // Calculate the global position of the circle's center
                b2Vec2 center = body->GetWorldPoint(circleShape->m_p);
                debugDraw_->DrawCircle(center, circleShape->m_radius, box2DRed);
            }
            // Add handling for other shape types (e.g., circle) as needed
        }
    }

    // Loop through birds if they are handled separately
    for (auto bird : birds_) {
    //const Bird* bird = GetBird(); // Assuming this method exists and returns a single bird for simplicity
        const auto birdBody = bird->getBody(); // Assuming getBody() returns a b2Body*
        for (const b2Fixture* f = birdBody->GetFixtureList(); f; f = f->GetNext()) {
            // Assuming birds are represented with a different color or shape
                if (f->GetType() == b2Shape::e_circle) {
                    const b2CircleShape* circleShape = static_cast<const b2CircleShape*>(f->GetShape());
                    // Get the radius of the circle
                    float radius = circleShape->m_radius;
                    // Calculate the global position of the circle's center
                    b2Vec2 center = birdBody->GetWorldPoint(circleShape->m_p);
                    // Convert Box2D position to SFML position (if necessary, depending on your coordinate system)
                    sf::Vector2f sfCenter(center.x, center.y);
                    // Convert Box2D radius to SFML radius (if necessary, depending on your scale factor)
                    float sfRadius = radius;
                    // Draw the circle with SFMLDebugDraw
                    debugDraw_->DrawCircle(center, sfRadius, box2DGreen);
                }
        }
    }
   
}

void World::setDebugDraw(SFMLDebugDraw* debugDraw) {
    this->debugDraw_ = debugDraw;
};