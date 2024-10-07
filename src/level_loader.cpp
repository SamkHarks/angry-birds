#include "level_loader.hpp"
#include "utils.hpp"
#include "world.hpp"

// half width and half height of the ground
b2Vec2 GROUND_DIMENSIONS = utils::SfToB2(sf::Vector2f(VIEW_WIDTH, 50.f));
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


LevelLoader::LevelLoader(World& world) : level_(world) {}


void LevelLoader::setLevelName(json levelJson) {
    level_.levelName_ = levelJson["name"];
}



std::vector<Bird::Type> LevelLoader::readBirdList(json levelJson) {
    std::vector<Bird::Type> birdList;
    for (const auto& birdType : levelJson["birds"]["list"]) {
        if (birdType == "R") {
            birdList.push_back(Bird::Type::Red);
        } else if (birdType == "L") {
            birdList.push_back(Bird::Type::Blue);
        } else if (birdType == "G") {
            birdList.push_back(Bird::Type::Green);
        } else {
            throw std::runtime_error("Invalid bird type. Should be one of R, B, G");
        }
    }
    return birdList;
}

b2Body* LevelLoader::createBody(const ObjectData& data) {
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

    b2Body *body = level_.world_->CreateBody(&body_def);

    return body;
}

void LevelLoader::createFixtureShape(ShapeData data, b2FixtureDef& fixtureDef, Object::Type& type, Shapes &shapes) {
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

void LevelLoader::createBird(Bird::Type birdType, b2Body* body, b2FixtureDef& fixtureDef) {
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
        default:
            throw std::runtime_error("Invalid bird type, bird type is one of R, L, G");
    }
    if (bird) {
        level_.addObject(bird);
        // Disable the bird initially in b2World
        body->SetEnabled(false);
        fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(bird);
    }
}

void LevelLoader::createObject(
    Object::Type objType,
    b2Body* body,
    b2FixtureDef& fixtureDef
) {
    Object *object = nullptr;
    switch (objType) {
        case Object::Type::Bird:
            // Bird is created with createBird function
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
        level_.addObject(object);
    }
}

void LevelLoader::saveHighScore(int score, const std::string& fileName) {
    std::string path = utils::getExecutablePath() + "/assets/levels/";
    std::ifstream inFile(path + fileName);
    if(!inFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + fileName);
    }
    json levelJson;
    inFile >> levelJson;
    inFile.close();

    levelJson["highScore"] = score;
    std::ofstream outFile(path + fileName);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + fileName);
    }
    outFile << levelJson.dump(4);
    outFile.close();
}

void LevelLoader::loadLevel(const std::string& fileName) {
    std::string path = utils::getExecutablePath() + "/assets/levels/";
    std::ifstream file(path + fileName);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + fileName);
    }
    // Read the level json from the file and close the file
    json levelJson;
    file >> levelJson;
    file.close();
    // Read and set level name and file name
    setLevelName(levelJson);
    level_.fileName_ = fileName;
    // Read and set high score
    int highScore = levelJson["highScore"];
    level_.scoreManager_.updateHighScore(highScore);
    // Read bird list
    std::vector<Bird::Type> birdList = readBirdList(levelJson);

    // Read birds and create Box2d bodies and fixtures
    for(const auto& birdType : birdList) {
        auto object = levelJson["birds"]["object"];
        auto jsonBody = object["body"];
        auto jsonShape = object["shape"];
        ObjectData data = jsonBody.template get<ObjectData>();
        b2Body *body = createBody(data);
        b2FixtureDef fixtureDef;
        Shapes shapes;
        ShapeData shapeData = jsonShape.template get<ShapeData>();
        createFixtureShape(shapeData, fixtureDef, data.type, shapes);
        createBird(birdType, body, fixtureDef);
        body->CreateFixture(&fixtureDef);
    }

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
        createObject(data.type, body, fixtureDef);
        body->CreateFixture(&fixtureDef);
    }
    // Set the total bird and pig count
    level_.totalBirdCount_ = level_.getRemainingBirdCount();
    level_.totalPigCount_ = level_.getRemainingPigCount();
}