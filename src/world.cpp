#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// half width and half height of the ground
b2Vec2 GROUND_DIMENSIONS = utils::SfToB2(sf::Vector2f(VIEW_WIDTH / 2.f, 50.f));
// half width and half height of the wall
b2Vec2 WALL_DIMENSONS = utils::SfToB2(sf::Vector2f(25.f, 150.f));

void from_json(const json& j, b2Vec2& vec) {
    vec.x = j.at(0).get<float>();
    vec.y = j.at(1).get<float>();
}

void to_json(json& j, const ShapeData& data) {
    if (data.shapeType == b2Shape::e_circle) {
        j = json{
            {"shapeType", data.shapeType},
            {"shapePosition", {data.shapePosition.x, data.shapePosition.y}},
            {"radius", data.radius},
            {"density", data.density},
            {"friction", data.friction},
            {"restitution", data.restitution}
        };
    } else {
        j = json{
            {"shapeType", data.shapeType},
            {"density", data.density},
            {"friction", data.friction},
            {"restitution", data.restitution}
        };
    }
}

void from_json(const json& j, ShapeData& data) {
    data.shapeType = j.at("shapeType").get<int>();
    if (data.shapeType == b2Shape::e_circle) {
        j.at("shapePosition").get_to(data.shapePosition);
        data.radius = j.at("radius").get<float>();
    }
    j.at("density").get_to(data.density);
    j.at("friction").get_to(data.friction);
    j.at("restitution").get_to(data.restitution);
}

// Define to_json and from_json functions for ObjectData
void to_json(json& j, const ObjectData& data) {
    if (data.type == Object::Type::Ground) {
        // Early return for ground object
        j = json{
            {"type", "G"},
        };
        return;
    }
    j = json{
        {"type", data.type},
        {"position", {data.position.x, data.position.y}},
        {"angle", data.angle},
        {"angularVelocity", data.angularVelocity},
        {"linearVelocity", {data.linearVelocity.x, data.linearVelocity.y}},
        {"angularDamping", data.angularDamping},
        {"linearDamping", data.linearDamping},
        {"gravityScale", data.gravityScale},
        {"bodyType", data.bodyType},
        {"awake", data.awake}
    };
}

void from_json(const json& j, ObjectData& data) {
    std::string typeStr;
    j.at("type").get_to(typeStr);
    switch (typeStr[0]) {
        case 'B':
            data.type = Object::Type::Bird;
            break;
        case 'G':
            // Early return for ground object
            data.type = Object::Type::Ground;
            data.bodyType = j.at("bodyType").get_to(data.bodyType);
            data.position = b2Vec2(GROUND_DIMENSIONS.x, 0);
            return;
        case 'P':
            data.type = Object::Type::Pig;
            break;
        case 'W':
            data.type = Object::Type::Wall;
            break;
        default:
            throw std::runtime_error("Invalid object type occured while reading from json. Should be one of B, G, P, W");
    }
    j.at("position").get_to(data.position);
    j.at("angle").get_to(data.angle);
    j.at("angularVelocity").get_to(data.angularVelocity);
    j.at("linearVelocity").get_to(data.linearVelocity);
    j.at("angularDamping").get_to(data.angularDamping);
    j.at("linearDamping").get_to(data.linearDamping);
    j.at("gravityScale").get_to(data.gravityScale);
    j.at("bodyType").get_to(data.bodyType);
    j.at("awake").get_to(data.awake);
}

World::World() : gravity_(0.0f, -9.8f) {
    world_ = new b2World(gravity_);
    cannon_ = new Cannon(
            "/assets/images/cannon_barrel.png",
            "/assets/images/cannon_wheel.png",
            "/assets/fonts/BerkshireSwash-Regular.ttf"
        );
    if(!utils::loadFromFile(background_image_, "/assets/images/background.jpg")) {
        std::cerr << "Failed to load image for the world" << std::endl;
    } else {
        background_.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
        background_.setTexture(&background_image_);
        background_.setPosition(0,0);
    }
}

World::~World() {
    for (auto object : objects_) {
        delete object;
    }
    for (auto bird : birds_) {
        delete bird;
    }
    delete cannon_;
    delete world_;
}

void World::addObject(Object *object) {
    if (object->getType() == Object::Type::Bird) {
        birds_.push_back(dynamic_cast<Bird *>(object));
    } else {
        objects_.push_back(object);
    }
}

std::vector<Bird::Type> World::readBirdList(json levelJson) {
    std::vector<Bird::Type> birdList;
    for (const auto& birdType : levelJson["birds"]) {
        if (birdType == "R") {
            birdList.push_back(Bird::Type::Red);
        } else if (birdType == "B") {
            birdList.push_back(Bird::Type::Blue);
        } else if (birdType == "G") {
            birdList.push_back(Bird::Type::Green);
        } else {
            throw std::runtime_error("Invalid bird type. Should be one of R, B, G");
        }
    }
    return birdList;
}

void World::loadLevel(const std::string& filename) {
    std::string path = utils::getExecutablePath() + "/assets/levels/";
    std::ifstream file(path + filename);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + filename);
    }
    // Read the level json from the file and close the file
    json levelJson;
    file >> levelJson;
    file.close();
    // Read and set level name
    setLevelName(levelJson);
    // Read bird list
    std::vector<Bird::Type> birdList = readBirdList(levelJson);
    // Read objects and create Box2D bodies and fixtures
    for (const auto& obj : levelJson["objects"]) {
        auto jsonBody = obj["body"];
        auto jsonShape = obj["shape"];
        ObjectData data = jsonBody.template get<ObjectData>();
        b2Body *body = createBody(data);
        b2FixtureDef fixtureDef;
        Shapes shapes;
        ShapeData shapeData = jsonShape.template get<ShapeData>();
        createFixtureShape(shapeData, fixtureDef, data.type, shapes);
        createObject(data.type, birdList, body, fixtureDef);
        body->CreateFixture(&fixtureDef);
    }
    // Set the total bird and pig count
    totalBirdCount_ = getRemainingBirdCount();
    totalPigCount_ = getRemainingPigCount();
}

std::tuple<int, float> World::getScoreAndStars() const {
    int pigsLeft = getRemainingPigCount();
    int birdsLeft = getRemainingBirdCount();
    float maxScore = (totalPigCount_ + totalBirdCount_) * 1000.f;
    float score = 1000.f * ((totalPigCount_ - pigsLeft) + birdsLeft);
    float scoreRatio = score / maxScore;
    if (scoreRatio >= 0.9) {
        return std::make_tuple(3, score);
    } else if (scoreRatio >= 0.6) {
        return std::make_tuple(2, score);
    } else if (scoreRatio >= 0.5) {
        return std::make_tuple(1, score);
    } else {
        return std::make_tuple(0, score);
    }

}

int World::getRemainingPigCount() const {
    int count = 0;
    for (auto object : objects_) {
        if (object->getType() == Object::Type::Pig) {
            count++;
        }
    }
    return count;
}

int World::getRemainingBirdCount() const {
    auto count = birds_.size();
    if (count > 0) {
        Bird *bird = birds_.front();
        if (bird->isLaunched()) {
            count--;
        }
    }
    return count;
}

void World::setLevelName(json levelJson) {
    levelName_ = levelJson["name"];
    fileName_ = levelJson["file"];
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

void World::createFixtureShape(ShapeData data, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes) {
    switch (data.shapeType) {
        case b2Shape::Type::e_circle: {
            shapes.circle.m_p = data.shapePosition;
            shapes.circle.m_radius = data.radius;
            fixtureDef.shape = &shapes.circle;
            break;
        }
        case b2Shape::Type::e_polygon:
             if (type == Object::Type::Ground) {
                shapes.polygon.SetAsBox(GROUND_DIMENSIONS.x, GROUND_DIMENSIONS.y);
             } else if (type == Object::Type::Wall) {
                shapes.polygon.SetAsBox(WALL_DIMENSONS.x, WALL_DIMENSONS.y);
             } else {
                throw std::runtime_error("Invalid object type");
             }
             fixtureDef.shape = &shapes.polygon;
            break;
        default:
            throw std::runtime_error("Unknown shape type");
    }
    // Read the rest of the fixture data, density, friction, restitution
    fixtureDef.friction = data.friction;
    fixtureDef.restitution = data.restitution;
    if (type == Object::Type::Ground) {
        return;
    } else {
        fixtureDef.density = data.density;
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
            object = new Ground(body, GROUND_DIMENSIONS.x, GROUND_DIMENSIONS.y);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Pig:
            object = new Pig(body, fixtureDef.shape->m_radius);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Wall:
            object = new Wall(body, WALL_DIMENSONS.x, WALL_DIMENSONS.y);
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
    if (birds_.empty()) {
        return nullptr;
    }
    return birds_.front();
}

const Bird* World::GetBird() const {
    if (birds_.empty()) {
        return nullptr;
    }
    return birds_.front();
}

void World::draw(sf::RenderWindow &window) const {
    window.draw(background_);
    for (auto object : objects_) {
        window.draw(object->getSprite());
    }
    const Bird* bird = GetBird();
    if (bird != nullptr && bird->isLaunched()) {
        const sf::Sprite& sprite = bird->getSprite();
        window.draw(sprite);
    }
    cannon_->draw(window);
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

void World::removeObject(Object *object) {
    world_->DestroyBody(object->getBody());
    objects_.remove(object);
    delete object;
}

void World::removeBird() {
    if (!birds_.empty()) {
        Bird* bird = birds_.front();
        world_->DestroyBody(bird->getBody());
        birds_.pop_front();
        delete bird;
    }
}

Cannon* World::getCannon() {
    return cannon_;
}

void World::resetBird() {
    Bird* bird = GetBird();
    if (bird != nullptr) {
        b2Body* body = bird->getBody();
        body->SetTransform(BIRD_INITIAL_POSITION, 0);
        body->SetLinearVelocity(b2Vec2_zero);
        body->SetAngularVelocity(0);
        body->SetAwake(false);
        bird->setLaunched(false);
    }
}

void World::setIsSettled(bool isSettled) {
    isSettled_ = isSettled;
}

bool World::getIsSettled() const {
    return isSettled_;
}

void World::resetLevel() {
    // Destroy all bodies in the Box2D world
    for (b2Body* body = world_->GetBodyList(); body != nullptr; ) {
        b2Body* nextBody = body->GetNext();
        world_->DestroyBody(body);
        body = nextBody;
    }

    // Clear the objects
    for (auto object : objects_) {
        delete object;
    }
    objects_.clear();

    // Clear the birds
    for (auto bird : birds_) {
        delete bird;
    }
    birds_.clear();

    // Reset cannon
    cannon_->reset();

    // set world to not settled
    isSettled_ = false;

    // Reload the level
    loadLevel(fileName_);
}