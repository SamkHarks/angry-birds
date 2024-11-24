#include "level_editor.hpp"
#include "utils.hpp"

// half width and half height of the wall
sf::Vector2f WALL_INITIAL_SF_DIM(25, 150);
b2Vec2 WALL_INITIAL_DIM = utils::SfToB2(WALL_INITIAL_SF_DIM);

LevelEditor::LevelEditor() : levelCreator_() {
    // Initialize buttons
    buttonGroups_.init();

    // Initialize Settings
    const IconButton& iconButton = buttonGroups_.iconButtons[1];
    settings_.init(iconButton);

    // Add ground
    ObjectData data = createObjectData(Object::Type::Ground);
    ShapeData shapeData = createShapeData(Object::Type::Ground);
    if (!createLevelObject(data, shapeData, ground_)) {
        throw std::runtime_error("Failed to create ground object");
    }

    // Add cannon
    cannon_.init();
    // Initialize notifications
    notifications_.init();
}

void LevelEditor::handleResize() {
    cannon_.handleResize();
    buttonGroups_.handleResize();
    settings_.handleResize();
    notifications_.handleResize();
    // Update ground
    b2Vec2 position = ground_.data.position;
    sf::Sprite& sprite = ground_.sprite;
    sf::Vector2f centerPosition = utils::B2ToSfCoords(position);
    float heightSf = 100.f;
    sprite.setPosition(0, centerPosition.y - heightSf * 1.2f);
    // Update objects
    for (auto& object : objects_) {
        object.sprite.setPosition(utils::B2ToSfCoords(object.data.position));
    }
}


void LevelEditor::draw(sf::RenderWindow& window) const {
    // Draw ground
    window.draw(ground_.sprite);
    // Draw editor buttons
    for (const auto& button : buttonGroups_.editorButtons) {
        window.draw(button.shape);
        window.draw(button.text);
    }
    // Draw icon buttons
    for (const auto& button : buttonGroups_.iconButtons) {
        window.draw(button.shape);
    }
    // Draw objects
    for (const auto& object : objects_) {
        if (settings_.isChecked(CheckboxGroup::Type::SHOW_DELETE_BUTTONS) && object.hasDeleteButton) {
            window.draw(object.deleteButton);
        }
        window.draw(object.sprite);

    }
    // Draw cannon to help place objects in the level
    cannon_.draw(window);
    // Draw notifications if any
    notifications_.draw(window);
    // Draw settings if open
    settings_.draw(window);
}

void LevelEditor::update() {
    // clear errors after 5 seconds
    if (notifications_.isDisplayed() && notifications_.clock.getElapsedTime().asSeconds() > 5) {
        notifications_.clearNotifications();
    } 
    settings_.update(selectedItem_ == static_cast<int>(Item::SETTINGS));
}

int LevelEditor::getItemAtPosition(const sf::Vector2f& mousePosition) const {
    for (int i = 0; i < objects_.size(); i++) {
        // Get the sprite's inverse transform (to convert global coordinates to local)
        sf::Transform inverseTransform = objects_[i].sprite.getInverseTransform();
        
        // Convert the global mouse position to the sprite's local coordinates
        sf::Vector2f localMousePosition = inverseTransform.transformPoint(mousePosition);
        
        // Check if the local mouse position is within the sprite's local (unrotated) bounds
        if (objects_[i].sprite.getLocalBounds().contains(localMousePosition)) {
            return BUTTONS + i;
        } else if (objects_[i].deleteButton.getGlobalBounds().contains(mousePosition)
            && settings_.isChecked(CheckboxGroup::Type::SHOW_DELETE_BUTTONS)
            && objects_[i].hasDeleteButton
        ) {
            return BUTTONS + objects_.size() + i;
        }
    }
    const auto& editorButtons = buttonGroups_.editorButtons;
    for (int i = 0; i < EDITOR_BUTTONS; i++) {
        if (editorButtons[i].shape.getGlobalBounds().contains(mousePosition) || editorButtons[i].text.getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }

    for (int i = 0; i < ICON_BUTTONS; i++) {
        if (buttonGroups_.iconButtons[i].shape.getGlobalBounds().contains(mousePosition)) {
            return EDITOR_BUTTONS + i;
        }
    }
    // Check if settings box is clicked and return the index
    if (settings_.isHovered(mousePosition)) {
        return static_cast<int>(Item::SETTINGS);
    }
    return -1;
}

bool LevelEditor::handleMouseClick(const sf::Vector2f& mousePosition, const sf::RenderWindow& window) {
    int itemIndex = getItemAtPosition(mousePosition);
    if (itemIndex >= 0) {
        selectedItem_ = itemIndex;
        Item item = convertIndexToItem();
        if (item == Item::OBJECT) {
            isDragging_ = true;
            int index = getObjectIndex(Item::OBJECT);
            dragOffsets_.objectDragOffset = objects_[index].sprite.getPosition() - mousePosition;
            dragOffsets_.deleteDragOffset = objects_[index].deleteButton.getPosition() - mousePosition;
        } else if (item == Item::SAVE) {
            saveLevel(window);
        } else if (item == Item::SETTINGS) {
            settings_.handleMouseClick(mousePosition);
        } else if (item == Item::DELETE_OBJECT) {
            updateButtons(false);
            removeObject();
        } else {
            createObject();
        }
        return true;
    }
    return false;
}

void LevelEditor::handleKeyPress(const sf::Keyboard::Key& key) {
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
            // Handle rotation and scaling of wall
            if (objects_[getObjectIndex(item)].data.type == Object::Type::Wall) {
                handleWallButtonPress(key);
            }
            break;
        }
        case Item::DELETE_OBJECT: {
            if (key == sf::Keyboard::Key::Delete) {
                updateButtons(false);
                removeObject();
            }
            break;
        }
    }
}

void LevelEditor::handleWallButtonPress(const sf::Keyboard::Key& key) {
    int index = getObjectIndex(Item::OBJECT);
    LevelObject& object = objects_[index];
    sf::Sprite &sprite = object.sprite;
    sf::Sprite &deleteButton = object.deleteButton;
    float angle = sprite.getRotation();
    const sf::Vector2f scale = sprite.getScale();
    switch (key) {
        case sf::Keyboard::Key::R:
            sprite.setRotation(angle + 1);
            setDeleteButtonPosition(sprite, object);
            deleteButton.setRotation(sprite.getRotation());
            break;
        case sf::Keyboard::Key::T:
            sprite.setRotation(angle - 1);
            setDeleteButtonPosition(sprite, object);
            deleteButton.setRotation(sprite.getRotation());
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
    if (angle != sprite.getRotation()) {
        isPressed_ = true;
    } else if (scale != sprite.getScale()) {
        isPressed_ = true;
        setDeleteButtonPosition(sprite, object);
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
        int index = getObjectIndex(Item::OBJECT);
        objects_[index].sprite.setPosition(mousePosition + dragOffsets_.objectDragOffset);
        objects_[index].deleteButton.setPosition(mousePosition + dragOffsets_.deleteDragOffset);
    } else {
        if (isPressed_) return; // Prevent object from being unselected while rotating or scaling
        int hoveredItem = getItemAtPosition(mousePosition);
        if (hoveredItem == -1) {
            updateItem(false);
            selectedItem_ = -1;
            return;
        }
        updateItem(false);
        selectedItem_ = hoveredItem;
        updateItem(true);
        // Handle settings hover
        const Item item = convertIndexToItem();
        if (item == Item::SETTINGS) settings_.handleMouseMove(mousePosition);
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

void LevelEditor::removeObject() {
    auto index = getObjectIndex(Item::DELETE_OBJECT);
    auto id = objects_[index].id;
    bool isChanged = false;
    for (auto &object : objects_) {
        isChanged = object.intersectingObjects.erase(id) || isChanged;
    }
    objects_.erase(objects_.begin() + index);
    if (isChanged) {
        updateIntersectingColors();
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
    LevelObject& object = objects_[getObjectIndex(Item::OBJECT)];
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
        setDeleteButtonPosition(sprite, object);
    }
    // Check if the object intersects other objects
    bool isChanged = false;
    for (auto& otherObject : objects_) {
        if (&object == &otherObject) {
            continue; // Skip the current object
        }
        if (utils::checkOBBCollision(sprite, otherObject.sprite)) {
            auto first = object.intersectingObjects.insert(otherObject.id).second;
            auto second = otherObject.intersectingObjects.insert(object.id).second;
            isChanged = isChanged || first || second;
        } else {
            auto first = object.intersectingObjects.erase(otherObject.id);
            auto second = otherObject.intersectingObjects.erase(object.id);
            isChanged = isChanged || first || second;
        }
    }
    // Update colors if the intersecting status has changed
    if (isChanged) {
        updateIntersectingColors();
    }
    
}

void LevelEditor::setDeleteButtonPosition(const sf::Sprite& sprite, LevelObject& object) {
    sf::Sprite& deleteButton = object.deleteButton;
    if (object.data.type == Object::Type::Wall) {
        sf::Transform transform = sprite.getTransform();
        sf::Vector2f position = sf::Vector2f(0,0);
        sf::Vector2f globalPosition = transform.transformPoint(position);
        deleteButton.setPosition(globalPosition);
    } else {
        sf::Vector2f position = sprite.getPosition() + (deleteButton.getGlobalBounds().width / 2) * sf::Vector2f(1, -2.8);
        object.deleteButton.setPosition(position);
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
            auto& editorButtons = buttonGroups_.editorButtons;
            editorButtons[selectedItem_].text.setFillColor(isSelected ? LIME_GREEN : sf::Color::White);
            auto scale = isSelected ? 1.05f : 1.f;
            editorButtons[selectedItem_].shape.setScale(scale, scale);
            break;
        }
        case Item::SETTINGS:
        case Item::SAVE: {
            int index = getIconButtonIndex();
            auto scale = isSelected ? 1.05f : 1.f;
            buttonGroups_.iconButtons[index].shape.setScale(scale, scale);
            break;
        }
        case Item::OBJECT: {
            int index = getObjectIndex(item);
            LevelObject& object = objects_[index];
            if (object.isIntersecting()) {
                object.sprite.setColor(sf::Color::Red);
            } else {
                object.sprite.setColor(isSelected ? LIME_GREEN : sf::Color::White);
            }
            break;
        }
        case Item::DELETE_OBJECT: {
            int index = getObjectIndex(item);
            LevelObject& object = objects_[index];
            auto scale = isSelected ? 0.55f : 0.5f;
            object.deleteButton.setScale(scale, scale);
            break;
        }
        default:
            break;
    }

}

void LevelEditor::updateIntersectingColors() {
    for (int i = 0; i < objects_.size(); i++) {
        LevelObject& object = objects_[i];
        if (object.isIntersecting()) {
            object.sprite.setColor(sf::Color::Red);
        } else if (i + BUTTONS == selectedItem_) {
            object.sprite.setColor(LIME_GREEN);
        } else {
            object.sprite.setColor(sf::Color::White);
        }
    }
}

LevelEditor::Item LevelEditor::convertIndexToItem() const {
    switch (selectedItem_) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            return static_cast<Item>(selectedItem_);
        default: {
            int size = objects_.size();
            if (selectedItem_ >= BUTTONS && selectedItem_ < size + BUTTONS) {
                return Item::OBJECT;
            } else if (selectedItem_ >= BUTTONS + size && selectedItem_ < size * 2 + BUTTONS) {
                return Item::DELETE_OBJECT;
            } else {
                return Item::UNDEFINED;
            }
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

sf::Sprite LevelEditor::addDeleteButton(const sf::Sprite& sprite, const Object::Type& type) {
    if (type == Object::Type::Ground) {
        return sf::Sprite(); // Ground objects do not have delete buttons
    }
    sf::Sprite deleteButton;
    deleteButton.setTexture(ResourceManager::getInstance().getTexture("/assets/images/delete_button1.png"));
    deleteButton.setScale(0.5, 0.5);
    deleteButton.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
    sf::Vector2f position;
    if (type == Object::Type::Pig) {
        position = sprite.getPosition() + (deleteButton.getGlobalBounds().width / 2) * sf::Vector2f(1, -2.8);
        deleteButton.setPosition(position);
    } else {
        position = sf::Vector2f(0,0);
        sf::Vector2f localOffset(position);
        sf::Transform transform = sprite.getTransform();
        sf::Vector2f globalPosition = transform.transformPoint(localOffset);
        deleteButton.setPosition(globalPosition);
    }

    return deleteButton;
}

bool LevelEditor::createLevelObject(const ObjectData& data, const ShapeData& shapeData, LevelObject& object) {
    sf::Sprite sprite;
    if (createSprite(data, sprite)) {
        object.sprite = sprite;
        object.data = data;
        object.shapeData = shapeData;
        object.id = NEXT_ID++;
        object.deleteButton = addDeleteButton(sprite, data.type);
        object.hasDeleteButton = object.data.type != Object::Type::Ground;
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
    if (item == Item::UNDEFINED || item == Item::SAVE || item == Item::OBJECT || item == Item::SETTINGS) {
        return; // Don't update buttons for undefined items or save/settings buttons and objects
    }
    int buttonIndex = selectedItem_;
    if (item == Item::DELETE_OBJECT) {
        Object::Type type = objects_[getObjectIndex(item)].data.type;
        buttonIndex = type == Object::Type::Pig ? 3 : 4;
    }
    auto & editorButtons = buttonGroups_.editorButtons;
    if (!isAdded && editorButtons[buttonIndex].count == 0) {
        return; // Prevent negative count
    }
    int count = isAdded ? 1 + editorButtons[buttonIndex].count++ : -1 + editorButtons[buttonIndex].count--;
    editorButtons[buttonIndex].text.setString(std::to_string(count));
}
const int LevelEditor::getObjectIndex(const Item& item) const {
    assert(item == Item::OBJECT || item == Item::DELETE_OBJECT);
    if (item == Item::OBJECT) {
        assert(selectedItem_ >= BUTTONS && selectedItem_ < BUTTONS + objects_.size());
        return selectedItem_ - BUTTONS;
    } else {
        assert(selectedItem_ >= BUTTONS + objects_.size() && selectedItem_ < BUTTONS + 2 * objects_.size());
        return selectedItem_ - BUTTONS - objects_.size();
    }
}

const int LevelEditor::getIconButtonIndex() const {
    assert(selectedItem_ >= EDITOR_BUTTONS && selectedItem_ < EDITOR_BUTTONS + ICON_BUTTONS);
    return selectedItem_ - EDITOR_BUTTONS;
}

void LevelEditor::saveLevel(const sf::RenderWindow& window) {
    bool noPigs = true;
    bool hasErrors = false;
    for (const auto& object : objects_) {
        if (object.data.type == Object::Type::Pig) {
            noPigs = false;
            break;
        }
    }
    if (noPigs) {
        notifications_.addNotification("Level should contain at least one Pig", Notifications::Type::ERROR_MESSAGE);
        hasErrors = true;
    }
    if (birdList_.empty()) {
        hasErrors = true;
        notifications_.addNotification("Level should contain at least one Bird", Notifications::Type::ERROR_MESSAGE);
    }
    for (const auto& object : objects_) {
        if (object.isIntersecting()) {
            hasErrors = true;
            notifications_.addNotification("Objects cannot overlap each other", Notifications::Type::ERROR_MESSAGE);
            break;
        }
    }
    if (hasErrors) {
        return; // Early exit with errors
    }
    std::vector<LevelObject> levelObjects;
    levelObjects.push_back(ground_);
    for (const auto& object : objects_) {
        levelObjects.push_back(object);
    }
    
    levelCreator_.createLevel(birdList_, levelObjects);
    levelCreator_.captureScreenShot(window);
    notifications_.addNotification("Level and screenshot saved successfully", Notifications::Type::MESSAGE);
}

void LevelEditor::captureLevelImage(const sf::RenderWindow& window) {
    levelCreator_.captureScreenShot(window);
    notifications_.addNotification("Screenshot saved successfully", Notifications::Type::MESSAGE);
}

void LevelEditor::updateHUD(const sf::RenderWindow& window) {
    buttonGroups_.updatePosition(window);
    settings_.updatePosition(window);
    notifications_.updatePosition(window);
}
