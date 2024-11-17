#include "level_editor.hpp"
#include "utils.hpp"

const int BUTTONS = 5;
const int SHAPE_SIZE = 53;
const float PIG_RADIUS = 0.3;
const sf::Vector2f PIG_INITIAL_POSITION(50,50);
const sf::Vector2f WALL_INITIAL_POSITION(150,150);
// half width and half height of the wall
sf::Vector2f WALL_INITIAL_SF_DIM(25, 150);
b2Vec2 WALL_INITIAL_DIM = utils::SfToB2(WALL_INITIAL_SF_DIM);

LevelEditor::LevelEditor() : levelCreator_() {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    // Helper lambda for file paths
    auto getFilePath = [&](int i) {
        switch (i) {
            case 0:
                return "/assets/images/red_bird.png";
            case 1:
                return "/assets/images/blue_bird.png";
            case 2:
                return "/assets/images/green_bird.png";
            case 3:
                return "/assets/images/pig.png";
            case 4:
                return "/assets/images/box.png";
            default:
                return "";
        }
    };
    int offset = VIEW.getCenter().x - 200;
    sf::Font &font = resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    for (int i = 0; i < BUTTONS; ++i) {
        sf::RectangleShape shape;
        shape.setTexture(&resourceManager.getTexture(getFilePath(i)));
        shape.setSize(sf::Vector2f(SHAPE_SIZE, SHAPE_SIZE));
        shape.setPosition(offset, 15);
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(2);
        text.setString(std::to_string(0));
        text.setPosition(offset + (i < 3 ? 20 : 15), 60);
        offset += 80;
        Button button = {shape,text};
        buttons_.push_back(button);
    }

    // Add ground
    ObjectData data = createObjectData(Object::Type::Ground);
    ShapeData shapeData = createShapeData(Object::Type::Ground);
    if (!createLevelObject(data, shapeData, ground_)) {
        throw std::runtime_error("Failed to create ground object");
    }

    // Add cannon
    cannon_.init();
}


void LevelEditor::draw(sf::RenderWindow& window) const {
    // Draw ground
    window.draw(ground_.sprite);
    // Draw buttons
    for (const auto& button : buttons_) {
        window.draw(button.shape);
        window.draw(button.text);
    }
    // Draw objects
    for (const auto& object : objects_) {
        window.draw(object.sprite);
    }
    // Draw cannon to help place objects in the level
    cannon_.draw(window);
}

int LevelEditor::getItemAtPosition(const sf::Vector2f& mousePosition) const {
    for (int i = 0; i < BUTTONS; i++) {
        if (buttons_[i].shape.getGlobalBounds().contains(mousePosition) || buttons_[i].text.getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }
    for (int i = 0; i < objects_.size(); i++) {
        // Get the sprite's inverse transform (to convert global coordinates to local)
        sf::Transform inverseTransform = objects_[i].sprite.getInverseTransform();
        
        // Convert the global mouse position to the sprite's local coordinates
        sf::Vector2f localMousePosition = inverseTransform.transformPoint(mousePosition);
        
        // Check if the local mouse position is within the sprite's local (unrotated) bounds
        if (objects_[i].sprite.getLocalBounds().contains(localMousePosition)) {
            return BUTTONS + i;
        }
    }
    return -1;
}

bool LevelEditor::handleMouseClick(const sf::Vector2f& mousePosition) {
    int itemIndex = getItemAtPosition(mousePosition);
    if (itemIndex >= 0) {
        selectedItem_ = itemIndex;
        if (convertIndexToItem() == Item::OBJECT) {
            isDragging_ = true;
            dragOffset_ = objects_[getObjectIndex()].sprite.getPosition() - mousePosition;
        } else {
            createObject();
        }
        return true;
    }
    return false;
}

void LevelEditor::handleKeyPress(const sf::Keyboard::Key& key) {
    // TODO: Remove Enter later and add a save button to the UI instead
    if (key == sf::Keyboard::Key::Enter) {
        saveLevel();
        return;
    }
    Item item = convertIndexToItem();
    switch (item) {
        case Item::RED_BIRD:
        case Item::BLUE_BIRD:
        case Item::GREEN_BIRD:
            if (key == sf::Keyboard::Key::Delete) {
                updateButtons(false);
                removeBird(getBirdType(item));
            }
            break;
        case Item::OBJECT: {
            // Delete pig or wall
            if (key == sf::Keyboard::Key::Delete) {
                updateButtons(false);
                objects_.erase(objects_.begin() + getObjectIndex());
                break;
            }
            // Handle rotation and scaling of wall
            if (objects_[getObjectIndex()].data.type == Object::Type::Wall) {
                handleWallButtonPress(key);
            }
            break;
        }
    }
}

void LevelEditor::handleWallButtonPress(const sf::Keyboard::Key& key) {
    sf::Sprite &sprite = objects_[getObjectIndex()].sprite;
    float angle = sprite.getRotation();
    const sf::Vector2f scale = sprite.getScale();
    switch (key) {
        case sf::Keyboard::Key::R:
            sprite.setRotation(angle + 1);
            break;
        case sf::Keyboard::Key::T:
            sprite.setRotation(angle - 1);
            break;
        case sf::Keyboard::Key::A:
            if (scale.x < 0.15) {
                break; // Prevent the object from becoming too small
            }
            sprite.setScale(scale.x - 0.05, scale.y);
            break;
        case sf::Keyboard::Key::D:
            if (scale.x > 2.2) {
                break; // Prevent the object from becoming too large
            }
            sprite.setScale(scale.x + 0.05, scale.y);
            break;
        case sf::Keyboard::Key::W:
            if (scale.y > 2.2) {
                break; // Prevent the object from becoming too large
            }
            sprite.setScale(scale.x, scale.y + 0.05);
            break;
        case sf::Keyboard::Key::S:
            if (scale.y < 0.15) {
                break; // Prevent the object from becoming too small
            }
            sprite.setScale(scale.x, scale.y - 0.05);
            break;
        default:
            break;
    }
    // Set is pressed to true if the object has been rotated or scaled and update level object after key release
    if (angle != sprite.getRotation() || scale != sprite.getScale()) {
        isPressed_ = true;
    }
}

// Update object after key release if it has been rotated or scaled
void LevelEditor::handleKeyRelease() {
    if (isPressed_) {
        isPressed_ = false;
        updateObject();
    }
}

void LevelEditor::handleMouseMove(const sf::Vector2f& mousePosition) {
    if (isDragging_) {
        objects_[getObjectIndex()].sprite.setPosition(mousePosition + dragOffset_);
    } else {
        int hoveredItem = getItemAtPosition(mousePosition);
        if (hoveredItem == -1) {
            updateItem(false);
            selectedItem_ = -1;
            return;
        }
        updateItem(false);
        selectedItem_ = hoveredItem;
        updateItem(true);
    }
}

void LevelEditor::removeBird(const Bird::Type& type) {
    for (auto it = birdList_.begin(); it != birdList_.end(); ++it) {
        // Find first bird of the given type and remove it and break
        if (*it == type) {
            birdList_.erase(it);
            break;
        }
    }
}

Bird::Type LevelEditor::getBirdType(const Item &item) const {
    assert(item == Item::RED_BIRD || item == Item::BLUE_BIRD || item == Item::GREEN_BIRD);
    if (item == Item::RED_BIRD) {
        return Bird::Type::Red;
    } else if (item == Item::BLUE_BIRD) {
        return Bird::Type::Blue;
    } else {
        return Bird::Type::Green;
    }
} 

// Update LevelObject's data and shapeData after modifying the object's sprite
void LevelEditor::updateObject() {
    LevelObject& object = objects_[getObjectIndex()];
    checkPosition(object);
    auto position = utils::SfToB2Coords(object.sprite.getPosition());
    if (object.data.position != position) {
        object.data.position = position;
    }
    if (object.data.type == Object::Type::Pig) {
        return; // Pigs cannot be rotated or scaled in the level editor
    }
    b2Vec2 dimensions = getDimensions(object);
    auto angle = utils::DegreesToRadians(object.sprite.getRotation());
    if (object.data.angle != angle) {
        object.data.angle = -angle;
    }
    if (object.shapeData.dimensions != dimensions) {
        object.shapeData.dimensions = dimensions;
    }
}

void LevelEditor::handleMouseRelease() {
    // Item::OBJECT is only draggable
    if (isDragging_) {
        isDragging_ = false;
        updateObject();
    }
}

b2Vec2 LevelEditor::getDimensions(const LevelObject& object) const {
    auto scale = object.sprite.getScale();
    auto width = object.sprite.getTextureRect().width;
    auto height = object.sprite.getTextureRect().height;
    auto newDimx = scale.x * width / (2 * SCALE);
    auto newDimy = scale.y * height / (2 * SCALE);
    return b2Vec2(newDimx, newDimy);
}

void LevelEditor::checkPosition(LevelObject& object) {
    sf::Sprite& sprite = object.sprite;
    auto newPosition = sprite.getPosition();
    auto bounds = sprite.getGlobalBounds();
    auto halfWidth = bounds.width / 2;
    auto halfHeight = bounds.height / 2;
    auto worldTop = -VIEW.getHeight() + 200;
    // Check if the object intersects 0
    if (newPosition.x < halfWidth) {
        newPosition.x = halfWidth;
    // Check if the object intersects the right side of the screen
    } else if (newPosition.x > WORLD_WIDTH - halfWidth) {
        newPosition.x = WORLD_WIDTH - halfWidth;
    // Check if the object intersects the top of the screen
    } 
    
    if (newPosition.y < worldTop + halfHeight) {
        newPosition.y = worldTop + halfHeight;
    // Check if the object intersects the ground
    } else if (newPosition.y > VIEW.getHeight() - 50 - halfHeight) {
        newPosition.y = VIEW.getHeight() - 50 - halfHeight;
    }

    if (newPosition != sprite.getPosition()) {
        sprite.setPosition(newPosition);
    }
    // Check if the object intersects other objects
    bool isIntersecting = object.isIntersecting;
    object.isIntersecting = false;
    for (auto& otherObject : objects_) {
        if (&object == &otherObject) {
            continue; // Skip the current object
        }
        if (utils::checkOBBCollision(sprite, otherObject.sprite)) {
           object.isIntersecting = true;
           break;
        }
       
    }
    // Change the color of the object if it intersects with another object or restore the color
    if (isIntersecting != object.isIntersecting) {
        updateItem(true);
    }
    
}


bool LevelEditor::isDragging() const {
    return isDragging_;
}

const int LevelEditor::getSelectedItem() const {
    return selectedItem_;
}

void LevelEditor::updateItem(bool isSelected) {
    const Item item = convertIndexToItem();
    switch (item) {
        case Item::RED_BIRD:
        case Item::BLUE_BIRD:
        case Item::GREEN_BIRD:
        case Item::PIG:
        case Item::WALL: {
            buttons_[selectedItem_].text.setFillColor(isSelected ? LIME_GREEN : sf::Color::White);
            auto bounds = buttons_[selectedItem_].shape.getGlobalBounds();
            auto scale = isSelected ? 1.05f : 1.f;
            buttons_[selectedItem_].shape.setScale(scale, scale);
            break;
        }
        case Item::OBJECT: {
            int index = getObjectIndex();
            LevelObject& object = objects_[index];
            if (object.isIntersecting) {
                object.sprite.setColor(sf::Color::Red);
            } else {
                object.sprite.setColor(isSelected ? LIME_GREEN : sf::Color::White);
            }
            break;
        }
        default:
            break;
    }

}

LevelEditor::Item LevelEditor::convertIndexToItem() const {
    switch (selectedItem_) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return static_cast<Item>(selectedItem_);
        default:
            if (selectedItem_ >= BUTTONS && selectedItem_ < objects_.size() + BUTTONS) {
                return Item::OBJECT;
            } else {
                return Item::UNDEFINED;
            }
    }
}

void LevelEditor::createObject() {
    LevelObject object;
    bool objectCreated = false;
    bool birdCreated = false;
    switch (convertIndexToItem()) {
        case Item::RED_BIRD:
            birdList_.push_back(Bird::Type::Red);
            birdCreated = true;
            break;
        case Item::GREEN_BIRD:
            birdList_.push_back(Bird::Type::Green);
            birdCreated = true;
            break;
        case Item::BLUE_BIRD:
            birdList_.push_back(Bird::Type::Blue);
            birdCreated = true;
            break;
        case Item::PIG:
            objectCreated = createLevelObject(createObjectData(Object::Type::Pig), createShapeData(Object::Type::Pig), object);
            break;
        case Item::WALL:
            objectCreated = createLevelObject(createObjectData(Object::Type::Wall), createShapeData(Object::Type::Wall), object);
            break;
        default:
            break;
    }

    if (objectCreated) {
        objects_.push_back(object);
        updateButtons(true);
    } else if (birdCreated) {
        updateButtons(true);
    }
}

ObjectData LevelEditor::createObjectData(Object::Type type) const {
    assert (type == Object::Type::Pig || type == Object::Type::Wall || type == Object::Type::Ground);
    ObjectData data;
    if (type == Object::Type::Ground) {
        data.type = Object::Type::Ground;
        data.position = b2Vec2(GROUND_DIMENSIONS.x, 0);
        data.bodyType = b2BodyType::b2_staticBody;
        return data; // Early return for ground object
    }

    if (type == Object::Type::Pig) {
        data.type = Object::Type::Pig;
        data.position = utils::SfToB2Coords(PIG_INITIAL_POSITION);
    } else {
        data.type = Object::Type::Wall;
        data.position = utils::SfToB2Coords(WALL_INITIAL_POSITION);
    }
    data.angle = 0;
    data.angularVelocity = 0;
    data.angularDamping = 0;
    data.awake = true;
    data.bodyType = b2BodyType::b2_dynamicBody;
    data.gravityScale = 1;
    data.linearDamping = 0;
    data.linearVelocity = b2Vec2(0,0);
    return data;
}

ShapeData LevelEditor::createShapeData(Object::Type type) const {
    assert (type == Object::Type::Pig || type == Object::Type::Wall || type == Object::Type::Ground);
    ShapeData data;
    switch (type) {
        case Object::Type::Pig:
            data.shapeType = 0;
            data.shapePosition = b2Vec2(0,0);
            data.radius = PIG_RADIUS;
            data.density = 1;
            data.friction = 1;
            data.restitution = 0.5;
            break;
        case Object::Type::Wall:
            data.shapeType = 2;
            data.dimensions = WALL_INITIAL_DIM;
            data.density = 1;
            data.friction = 1;
            data.restitution = 0.4;
            break;
        case Object::Type::Ground:
            data.shapeType = 2;
            data.dimensions = GROUND_DIMENSIONS;
            data.density = 1;
            data.friction = 0.5;
            data.restitution = 0.2;
            break;
        default:
            break;
    }
    return data;
}

bool LevelEditor::createLevelObject(const ObjectData& data, const ShapeData& shapeData, LevelObject& object) const {
    sf::Sprite sprite;
    if (createSprite(data, sprite)) {
        object.sprite = sprite;
        object.data = data;
        object.shapeData = shapeData;
        return true;
    }
    return false;
}

bool LevelEditor::createSprite(const ObjectData& data, sf::Sprite& sprite) const {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    switch (data.type) {
        case Object::Type::Pig: {
            sprite.setTexture(resourceManager.getTexture("/assets/images/pig.png"));
            int width = sprite.getTextureRect().width;
            int height = sprite.getTextureRect().height;
            sprite.setScale(PIG_RADIUS * SCALE * 2.f / (1.f * width), PIG_RADIUS * SCALE * 2.f / (1.f * height));
            sprite.setOrigin(width / 2.f, height / 2.f);
            sprite.setPosition(utils::B2ToSfCoords(data.position));
            return true;
        }
        case Object::Type::Wall: {
            sprite.setTexture(resourceManager.getTexture("/assets/images/box.png"));
            float width = static_cast<float>(sprite.getTextureRect().width);
            float height = static_cast<float>(sprite.getTextureRect().height);
            float scaleY = (2.f * WALL_INITIAL_DIM.y * SCALE) / height;
            float scaleX = (2.f * WALL_INITIAL_DIM.x * SCALE) / width;
            sprite.setScale(scaleX, scaleY);
            sprite.setOrigin(width / 2.f, height / 2.f);
            sprite.setPosition(utils::B2ToSfCoords(data.position));
            return true;
        }
        case Object::Type::Ground: {
            sprite.setTexture(resourceManager.getTexture("/assets/images/ground.png"));
            float width = static_cast<float>(sprite.getTextureRect().width);
            float height = static_cast<float>(sprite.getTextureRect().height);
            float heightSf = utils::B2ToSf(2.f * GROUND_DIMENSIONS.y);
            float scaleX = utils::B2ToSf(2.f * GROUND_DIMENSIONS.x) / width;
            float scaleY = heightSf / height;
            float scaleSfactor = std::max(scaleX, scaleY);
            sprite.setScale(scaleSfactor, scaleSfactor);
            sf::Vector2f centerPosition = utils::B2ToSfCoords(data.position);
            sprite.setPosition(0, centerPosition.y - heightSf);
            return true;
        }
        default:
            return false;
    }
}

void LevelEditor::updateButtons(bool isAdded) {
    Item item = convertIndexToItem();
    int buttonIndex = selectedItem_;
    if (item == Item::OBJECT) {
        Object::Type type = objects_[getObjectIndex()].data.type;
        buttonIndex = type == Object::Type::Pig ? 3 : 4;
    }
    if (!isAdded && buttons_[buttonIndex].count == 0) {
        return; // Prevent negative count
    }
    int count = isAdded ? 1 + buttons_[buttonIndex].count++ : -1 + buttons_[buttonIndex].count--;
    buttons_[buttonIndex].text.setString(std::to_string(count));
}
const int LevelEditor::getObjectIndex() const {
    assert(selectedItem_ >= BUTTONS && selectedItem_ < BUTTONS + objects_.size());
    return selectedItem_ - BUTTONS;
}

void LevelEditor::saveLevel() const {
    if (birdList_.empty() || objects_.empty()) {
        return ; // No birds in the level
    }
    for (const auto& object : objects_) {
        if (object.isIntersecting) {
            return; // Wall is intersecting with another object
        }
    }
    std::vector<LevelObject> levelObjects;
    levelObjects.push_back(ground_);
    for (const auto& object : objects_) {
        levelObjects.push_back(object);
    }
    
    levelCreator_.createLevel(birdList_, levelObjects);

}
