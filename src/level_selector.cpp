#include "level_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

LevelSelector::LevelSelector() {
    // Load font
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    std::vector<std::string> button_texts = { "Level 1", "Back" };
    for (int i = 0; i < button_texts.size(); ++i) {
        sf::Text text;
        text.setFont(font_);
        text.setCharacterSize(55);
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(5);
        text.setString(button_texts[i]);
        sf::FloatRect textBounds = text.getGlobalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(SCREEN_CENTER.x, 300 + i * 324);
        menuItems_.push_back(text);
    }

    // Load level image
    if (!utils::loadFromFile(levelImage_, "/assets/images/level1.png")) {
        throw std::runtime_error("Failed to load texture file: /assets/images/level1.png");
    }
    level_.setSize(sf::Vector2f(VIEW_WIDTH,VIEW_HEIGHT));
    level_.setTexture(&levelImage_);
    level_.setOrigin(level_.getGlobalBounds().width / 2, level_.getGlobalBounds().height / 2);
    level_.setScale(0.27f, 0.27f);
    level_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);

    // Load arrow textures
    leftArrow_ = ResourceManager::getInstance().getTexture("/assets/images/wooden_arrow.png");
    rightArrow_ = ResourceManager::getInstance().getTexture("/assets/images/wooden_arrow.png");
    buttons_.resize(2);
    for (int i = 0; i < 2; ++i) {
        buttons_[i].setSize(sf::Vector2f(100, 100));
        auto globalBounds = buttons_[i].getGlobalBounds();
        buttons_[i].setOrigin(globalBounds.width / 2, globalBounds.height / 2);
        buttons_[i].setTexture(i == 0 ? &leftArrow_ : &rightArrow_);
        buttons_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -270 : 280), SCREEN_CENTER.y + 20);
        if (i == 0) {
            buttons_[i].setRotation(180);
        }
    }

    // Load levels
    levels_.push_back({"Level 1", "level1.json"});
}

void LevelSelector::draw(sf::RenderWindow& window) const {
    for (const auto& menuItem : menuItems_) {
        window.draw(menuItem);
    }
    for (const auto& button : buttons_) {
        window.draw(button);
    }
    window.draw(level_);
}

const LevelSelector::Item LevelSelector::getSelectedItem() const {
    return selectedItem_;
}

void LevelSelector::setSelectedItem(Item nextItem) {
    updateItem(false);
    selectedItem_ = nextItem;
    updateItem(true);
}

LevelSelector::Item LevelSelector::getItemAtPosition(sf::Vector2f mousePosition) const {
    for (int i = 0; i < menuItems_.size(); i++) {
        if (menuItems_[i].getGlobalBounds().contains(mousePosition)) {
            return static_cast<Item>(i);
        }
    }
    for (int i = 0; i < buttons_.size(); i++) {
        if (buttons_[i].getGlobalBounds().contains(mousePosition)) {
            return i == 0 ? Item::PREV : Item::NEXT;
        }
    }
    if (level_.getGlobalBounds().contains(mousePosition)) {
        return Item::LEVEL;
    }
    return Item::UNDEFINED;
}

bool LevelSelector::handleMouseClick(sf::Vector2f mousePosition) {
    Item item = getItemAtPosition(mousePosition);
    if (item != Item::UNDEFINED) {
        return true;
    }
    return false;
}

void LevelSelector::handleMouseMove(sf::Vector2f mousePosition) {
    Item hoveredItem = getItemAtPosition(mousePosition);
    if (hoveredItem == Item::UNDEFINED) {
        return;
    }
    updateItem(false);
    selectedItem_ = hoveredItem;
    updateItem(true);
}

void LevelSelector::updateItem(bool isSelected) {
    switch (selectedItem_) {
        case Item::LEVEL:
        case Item::BACK: {
            const int index = static_cast<int>(selectedItem_);
            const float scale = isSelected ? 1.1f : 1.f;
            menuItems_[index].setFillColor(isSelected ? LIME_GREEN : sf::Color::White);
            menuItems_[index].setScale(scale, scale);
            break;
        }
        case Item::PREV:
        case Item::NEXT: {
            const int index = selectedItem_ == Item::PREV ? 0 : 1;
            const float scale = isSelected ? 1.1f : 1.f;
            buttons_[index].setScale(scale, scale);
            break;
        }
    }
}

Level& LevelSelector::getSelectedLevel() {
    return levels_[levelIndex_];
}

void LevelSelector::setLevelIndex(int index) {
    levelIndex_ = index;
}
