#include "level_loader.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "resource_manager.hpp"

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
            {"restitution", data.restitution},
            {"dimensions", {data.dimensions.x, data.dimensions.y}}
        };
    }
}

void from_json(const json& j, ShapeData& data) {
    data.shapeType = j.at("shapeType").get<int>();
    if (data.shapeType == b2Shape::e_circle) {
        j.at("shapePosition").get_to(data.shapePosition);
        data.radius = j.at("radius").get<float>();
    } else {
        j.at("dimensions").get_to(data.dimensions);
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
            {"bodyType", data.bodyType}
        };
        return;
    } else if (data.type == Object::Type::Pig) {
        j["type"] = "P";
    } else if (data.type == Object::Type::Wall) {
        j["type"] = "W";
    } else {
        j["type"] = "B";
    }
    j["position"] = {data.position.x, data.position.y};
    j["angle"] = data.angle;
    j["angularVelocity"] = data.angularVelocity;
    j["linearVelocity"] = {data.linearVelocity.x, data.linearVelocity.y};
    j["angularDamping"] = data.angularDamping;
    j["linearDamping"] = data.linearDamping;
    j["gravityScale"] = data.gravityScale;
    j["bodyType"] = data.bodyType;
    j["awake"] = data.awake;
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
    level_.levelIndex_ = levelJson["id"];
    level_.levelName_ = "Level " + std::to_string(level_.levelIndex_ + 1);
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
            throw std::runtime_error("Invalid bird type. Should be one of R, L, G");
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
             if (type == Object::Type::Ground || type == Object::Type::Wall) {
                shapes.polygon.SetAsBox(data.dimensions.x, data.dimensions.y);
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
    b2FixtureDef& fixtureDef,
    const ShapeData& shapeData
) {
    Object *object = nullptr;
    switch (objType) {
        case Object::Type::Bird:
            // Bird is created with createBird function
            break;
        case Object::Type::Ground:
            object = new Ground(body, shapeData.dimensions.x, shapeData.dimensions.y);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Pig:
            object = new Pig(body, fixtureDef.shape->m_radius);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        case Object::Type::Wall:
            object = new Wall(body, shapeData.dimensions.x, shapeData.dimensions.y);
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(object);
            break;
        default:
            throw std::runtime_error("Invalid object type");
    }
    if (object) {
        level_.addObject(object);
    }
}

void LevelLoader::saveHighScores(const std::vector<HighScore> &highScores, const std::string& fileName) {
    std::string path = utils::getExecutablePath() + "/assets/levels/";
    std::ifstream inFile(path + fileName);
    if(!inFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path + fileName);
    }
    json levelJson;
    inFile >> levelJson;
    inFile.close();
    // Convert the vector of high scores to a JSON array
    json highScoresJson = json::array();
    for (const auto& highScore : highScores) {
        highScoresJson.push_back({{"player", highScore.player}, {"score", highScore.score}});
    }
    // Update the highScores field in the JSON object
    levelJson["highScores"] = highScoresJson;
    // Write updated JSON object back to the file
    std::ofstream outFile(path + fileName);
    if(!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path + fileName);
    }
    outFile << levelJson.dump(4);
    outFile.close();
}

void LevelLoader::loadSfmlObjects(std::vector<Bird::Type>& birdList) {
    std::list<SfObject> sfObjects;
    ResourceManager& resourceManager = ResourceManager::getInstance();
    sf::Font& font = resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    // create pig sprite and text
    int offset = level_.cannon_->getTextWidth() + 40;
    SfObject pigObject;
    sf::Texture& pig = resourceManager.getTexture("/assets/images/pig.png");
    sf::Sprite pigSprite;
    pigSprite.setTexture(pig);
    pigSprite.setScale(0.09f, 0.09f);
    pigSprite.setPosition(offset, 10);
    sf::Text pigText;
    pigText.setFont(font);
    pigText.setCharacterSize(40);
    pigText.setFillColor(sf::Color::White);
    pigText.setOutlineColor(sf::Color::Black);
    pigText.setOutlineThickness(2);
    pigText.setString(std::to_string(level_.totalPigCount_));
    sf::FloatRect pigBounds = pigSprite.getLocalBounds();
    pigText.setPosition(offset + 20,45);
    // set the pig object
    pigObject.sprite = pigSprite;
    pigObject.text = pigText;
    pigObject.type = 'P';
    pigObject.count = level_.totalPigCount_;
    // add the pig object to the list
    sfObjects.push_back(pigObject);
    // get the bird counts
    std::vector<int> birdsLeft = {0,0,0};
    for(auto bird : birdList) {
        switch (bird)
        {
        case Bird::Type::Red:
            birdsLeft[0]++;
            break;
        case Bird::Type::Blue:
            birdsLeft[1]++;
            break;
        case Bird::Type::Green:
            birdsLeft[2]++;
            break;
        default:
            break;
        }
    }
    // use the lambda function to get the bird file path
    auto getFilePath = [&](int i) {
        switch (i) {
            case 0:
                return "/assets/images/red_bird.png";
            case 1:
                return "/assets/images/blue_bird.png";
            case 2:
                return "/assets/images/green_bird.png";
            default:
                return "";
        }
    };
    // create bird sprites and texts
    offset += 60;
    for(int i = 0; i < 3; i++) {
        int count = birdsLeft[i];
        if (count == 0) {
            continue;
        }
        sf::Texture& bird = resourceManager.getTexture(getFilePath(i));
        sf::Sprite birdSprite;
        birdSprite.setTexture(bird);
        birdSprite.setScale(0.1f, 0.1f);
        birdSprite.setPosition(offset, 10);
        sf::Text birdText;
        birdText.setFont(font);
        birdText.setCharacterSize(40);
        birdText.setFillColor(sf::Color::White);
        birdText.setOutlineColor(sf::Color::Black);
        birdText.setOutlineThickness(2);
        birdText.setString(std::to_string(count));
        birdText.setPosition(offset + 20, 45);
        offset += 60;
        // set the bird object
        SfObject birdObject;
        birdObject.sprite = birdSprite;
        birdObject.text = birdText;
        birdObject.type = i == 0 ? 'R' : i == 1 ? 'L' : 'G';
        birdObject.count = count;
        // add the bird object to the list
        sfObjects.push_back(birdObject);
    }
    // set the sf objects
    level_.sfObjects_ = sfObjects;
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
    // Read and set high scores
    int highScore = 0;
    auto highScores = levelJson["highScores"];
    std::vector<HighScore> highScoreList;
    for (auto score : highScores) {
        HighScore newHighScore;
        newHighScore.player = score["player"];
        newHighScore.score = score["score"];
        if (newHighScore.score > highScore) {
            highScore = newHighScore.score;
        }
        highScoreList.push_back(newHighScore);
    }
    level_.scoreManager_.setHighScores(highScoreList);
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
        createObject(data.type, body, fixtureDef, shapeData);
        body->CreateFixture(&fixtureDef);
    }
    // Set the total bird and pig count
    level_.totalBirdCount_ = level_.getRemainingBirdCount();
    level_.totalPigCount_ = level_.getRemainingPigCount();
    // Load resources
    loadSfmlObjects(birdList);
}


// LevelCreator class implementation
LevelCreator::LevelCreator() {}

void LevelCreator::createLevel(const std::vector<Bird::Type>& birdList, const std::vector<LevelObject>& objects) const {
    int levelCount = utils::countFilesInDirectory();
    std::string fileName = "level" + std::to_string(levelCount + 1) + ".json";
    std::string path = utils::getExecutablePath() + "/assets/levels/";
    std::ofstream file(path + fileName);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path + fileName);
    }
    json levelJson;
    levelJson["id"] = levelCount;
    levelJson["highScores"] = json::array();
    levelJson["birds"] = createBirds(birdList);
    levelJson["objects"] = createObjects(objects);
    file << levelJson.dump(4);
    file.close();
}

json LevelCreator::createBirdObject() const {
    json birdJson;
    ObjectData data;
    data.type = Object::Type::Bird;
    data.position = BIRD_INITIAL_POSITION;
    data.angle = 0;
    data.angularVelocity = 0;
    data.linearVelocity = b2Vec2(0, 0);
    data.angularDamping = 0;
    data.linearDamping = 0;
    data.gravityScale = 1;
    data.bodyType = b2BodyType::b2_dynamicBody;
    data.awake = false;
    birdJson["body"] = data;
    ShapeData shapeData;
    shapeData.shapeType = b2Shape::e_circle;
    shapeData.shapePosition = b2Vec2_zero;
    shapeData.radius = 0.3f;
    shapeData.density = 1;
    shapeData.friction = 1;
    shapeData.restitution = 0.4;
    birdJson["shape"] = shapeData;

    return birdJson;
}

json LevelCreator::createBirds(const std::vector<Bird::Type>& birdList) const {
    json birdsJson;
    json birdsArray = json::array();
    for (const auto& bird : birdList) {
        if (bird == Bird::Type::Red) {
            birdsArray.push_back("R");
        } else if (bird == Bird::Type::Blue) {
            birdsArray.push_back("L");
        } else if (bird == Bird::Type::Green) {
            birdsArray.push_back("G");
        } else {
            throw std::runtime_error("Invalid bird type. Should be one of R, L, G");
        }
    }
    birdsJson["list"] = birdsArray;
    birdsJson["object"] = createBirdObject();
    return birdsJson;
}

json LevelCreator::createObjects(const std::vector<LevelObject>& objects) const {
    json objectsArray = json::array();
    for (const auto& object : objects) {
        json objectJson;
        objectJson["body"] = object.data;
        objectJson["shape"] = object.shapeData;
        objectsArray.push_back(objectJson);
    }
    return objectsArray;
}

void LevelCreator::captureScreenShot(const sf::RenderWindow& window) const {
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    sf::Image image = texture.copyToImage();
    std::string path = utils::getExecutablePath() + "/assets/screenshots/";
    std::string fileName = "level" + std::to_string(utils::countFilesInDirectory()) + ".png";
    image.saveToFile(path + fileName);
}