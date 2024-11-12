#include "level_editor.hpp"

const int BUTTONS = 5;
const int SHAPE_SIZE = 53;
const float PIG_RADIUS = 0.3;
const sf::Vector2f PIG_INITIAL_POSITION(50,50);
const sf::Vector2f WALL_INITIAL_POSITION(150,150);
// half width and half height of the wall
b2Vec2 WALL_INITIAL_DIM = utils::SfToB2(sf::Vector2f(25.f, 150.f));

LevelEditor::LevelEditor() {
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
}


void LevelEditor::draw(sf::RenderWindow& window) const {
    // Draw buttons
    for (const auto& button : buttons_) {
        window.draw(button.shape);
        window.draw(button.text);
    }
    for (const auto& object : objects_) {
        window.draw(object.sprite);
    }
}

int LevelEditor::getItemAtPosition(const sf::Vector2f& mousePosition) const {
    for (int i = 0; i < BUTTONS; i++) {
        if (buttons_[i].shape.getGlobalBounds().contains(mousePosition) || buttons_[i].text.getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }
    for (int i = 0; i < objects_.size(); i++) {
        if (objects_[i].sprite.getGlobalBounds().contains(mousePosition)) {
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

void LevelEditor::updateObject() {
    objects_[getObjectIndex()].data.position = utils::SfToB2Coords(objects_[getObjectIndex()].sprite.getPosition());
}

void LevelEditor::handleMouseRelease() {
    isDragging_ = false;
    updateObject();
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
            objects_[index].sprite.setColor(isSelected ? LIME_GREEN : sf::Color::White);
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
    switch (convertIndexToItem()) {
        case Item::RED_BIRD:
            birdList_.push_back(Bird::Type::Red);
            objectCreated = true;
            break;
        case Item::GREEN_BIRD:
            birdList_.push_back(Bird::Type::Green);
            objectCreated = true;
            break;
        case Item::BLUE_BIRD:
            birdList_.push_back(Bird::Type::Blue);
            objectCreated = true;
            break;
        case Item::PIG:
            objectCreated = createLevelObject(createObjectData(Object::Type::Pig), object);
            break;
        case Item::WALL:
            objectCreated = createLevelObject(createObjectData(Object::Type::Wall), object);
            break;
        default:
            break;
    }

    if (objectCreated) {
        objects_.push_back(object);
        updateButtons();
    }
}

ObjectData LevelEditor::createObjectData(Object::Type type) const {
    assert (type == Object::Type::Pig || type == Object::Type::Wall);
    ObjectData data;
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

bool LevelEditor::createLevelObject(const ObjectData& data, LevelObject& object) const {
    sf::Sprite sprite;
    if (createSprite(data, sprite)) {
        object.sprite = sprite;
        object.data = data;
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
        default:
            return false;
    }
}

void LevelEditor::updateButtons() {
    int count = buttons_[selectedItem_].count++;
    buttons_[selectedItem_].text.setString(std::to_string(++count));
}
const int LevelEditor::getObjectIndex() const {
    assert(selectedItem_ >= BUTTONS && selectedItem_ < BUTTONS + objects_.size());
    return selectedItem_ - BUTTONS;
}