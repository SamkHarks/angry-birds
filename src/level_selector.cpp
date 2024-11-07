#include "level_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

const float STAR_SIZE = 100;

LevelSelector::LevelSelector() {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    // Load font
    font_ = resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
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
        text.setPosition(i == 0 ? SCREEN_CENTER.x - 50 : SCREEN_CENTER.x, (SCREEN_CENTER.y - 150) + i * 324);
        menuItems_.push_back(text);
    }

    // Load level image
    levelImage_ = resourceManager.getTexture("/assets/images/level1.png");
    level_.setSize(sf::Vector2f(VIEW.getWidth(),VIEW.getHeight()));
    level_.setTexture(&levelImage_);
    level_.setOrigin(level_.getGlobalBounds().width / 2, level_.getGlobalBounds().height / 2);
    level_.setScale(0.27f, 0.27f);
    level_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);

    // Load arrow textures
    leftArrow_ = resourceManager.getTexture("/assets/images/wooden_arrow.png");
    rightArrow_ = resourceManager.getTexture("/assets/images/wooden_arrow.png");
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
    levels_.push_back({"Level 1", "level1.json", "level1.png"});
    levels_.push_back({"Level 2", "level2.json", "level2.png"});

    // Load stars
    stars_.resize(4);
    float starSize = 100;
    for (size_t i = 0; i < stars_.size(); ++i) {
        stars_[i].setTexture(resourceManager.getTexture("/assets/images/stars_" + std::to_string(i) + ".png"));
        auto textureRect = stars_[i].getTextureRect();
        auto scaleFactor = std::min(STAR_SIZE / textureRect.width, STAR_SIZE / textureRect.height);
        stars_[i].setScale(scaleFactor, scaleFactor);
        stars_[i].setOrigin(textureRect.width / 2.f, textureRect.height / 2.f);
        stars_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x + 100, SCREEN_CENTER.y - 140));
    }

    // Set initial selected item to level
    selectedItem_ = Item::LEVEL;
    updateItem(true);
}

void LevelSelector::handleResize() {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    for (int i = 0; i < menuItems_.size(); i++) {
        menuItems_[i].setPosition(i == 0 ? SCREEN_CENTER.x - 50 : SCREEN_CENTER.x, (SCREEN_CENTER.y - 150) + i * 324);
    }
    for (int i = 0; i < buttons_.size(); i++) {
        buttons_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -270 : 280), SCREEN_CENTER.y + 20);
    }
    level_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);
    for (int i = 0; i < stars_.size(); i++) {
        stars_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x + 100, SCREEN_CENTER.y - 140));
    }
}

void LevelSelector::draw(sf::RenderWindow& window) const {
    for (const auto& menuItem : menuItems_) {
        window.draw(menuItem);
    }
    for (const auto& button : buttons_) {
        window.draw(button);
    }
    window.draw(level_);
    drawStars(window);
}

const LevelSelector::Item LevelSelector::getSelectedItem() const {
    return selectedItem_;
}

void LevelSelector::setSelectedItem(Item nextItem) {
    updateItem(false);
    selectedItem_ = nextItem;
    updateItem(true);
}

LevelSelector::Item LevelSelector::getItemAtPosition(const sf::Vector2f& mousePosition) const {
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
    if (stars_[starIndex_].getGlobalBounds().contains(mousePosition)) {
        return Item::LEVEL;
    }
    return Item::UNDEFINED;
}

bool LevelSelector::handleMouseClick(const sf::Vector2f& mousePosition) {
    Item item = getItemAtPosition(mousePosition);
    if (item != Item::UNDEFINED) {
        return true;
    }
    return false;
}

void LevelSelector::handleMouseMove(const sf::Vector2f& mousePosition) {
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
        case Item::LEVEL: {
            const int index = static_cast<int>(selectedItem_);
            const float scale = isSelected ? 1.1f : 1.f;
            menuItems_[index].setFillColor(isSelected ? LIME_GREEN : sf::Color::White);
            menuItems_[index].setScale(scale, scale);
            auto textureRect = stars_[starIndex_].getTextureRect();
            float starScale = std::min(STAR_SIZE / textureRect.width, STAR_SIZE / textureRect.height);
            starScale = isSelected ? 1.1f * starScale : starScale;
            stars_[starIndex_].setScale(starScale, starScale);
        }
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
    levelIndex_ = index % levels_.size();
}

void LevelSelector::setLevelText() {
    menuItems_[0].setString(levels_[levelIndex_].name);
}

void LevelSelector::setLevelImage() {
    levelImage_ = ResourceManager::getInstance().getTexture("/assets/images/" + levels_[levelIndex_].image);
    level_.setTexture(&levelImage_);
}

void LevelSelector::setLevel(Item item) {
    assert(item == Item::PREV || item == Item::NEXT);
    if (item == Item::PREV) {
        setLevelIndex(levelIndex_ - 1);
    } else {
        setLevelIndex(levelIndex_ + 1);
    }
    updateLevel();
}

bool LevelSelector::hasNextLevel() const {
    return levelIndex_ < levels_.size() - 1;
}

Level& LevelSelector::getNextLevel() {
    assert(hasNextLevel());
    setLevel(Item::NEXT);
    return levels_[levelIndex_];
}

void LevelSelector::setPlayer(const std::shared_ptr<Player>& player) {
    if (player == player_.lock()) {
        return;
    }
    player_ = player;
}

void LevelSelector::drawStars(sf::RenderWindow& window) const {
    if (auto player = player_.lock()) {
        window.draw(stars_[starIndex_]);
    }
}

void LevelSelector::setLevelStarIndex() {
    if (auto player = player_.lock()) {
        starIndex_ = levelIndex_ < player->levelsCompleted ? player->stars[levelIndex_] : 0;
    }
}

void LevelSelector::updateLevel() {
    setLevelStarIndex();
    setLevelText();
    setLevelImage();
}
