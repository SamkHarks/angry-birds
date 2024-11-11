#include "level_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

const float STAR_SIZE = 100;
const int SIGN_X_OFFSET = 530;
const int SIGN_Y_OFFSET = 200;
const int BUTTON_X_OFFSET = 280;

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

    // Load name sign
    sign_.resize(2);
    signText_.resize(3);
    for (int i = 0; i < 2; ++i) {
        sign_[i].setSize(sf::Vector2f(320, 320));
        std::string path = "/assets/images/wooden_sign" + std::to_string(i + 2) + ".png";
        sign_[i].setTexture(&resourceManager.getTexture(path));
        sign_[i].setOrigin(sign_[i].getGlobalBounds().width / 2, sign_[i].getGlobalBounds().height / 2);
        int xOffset = i == 0 ? -SIGN_X_OFFSET : SIGN_X_OFFSET;
        sign_[i].setPosition(SCREEN_CENTER.x + xOffset, SCREEN_CENTER.y + SIGN_Y_OFFSET);
        signText_[i].setFont(font_);
        signText_[i].setFillColor(sf::Color::White);
        signText_[i].setOutlineColor(sf::Color::Black);
        signText_[i].setOutlineThickness(3);
    }
    signText_[2].setFont(font_);
    signText_[2].setFillColor(sf::Color::White);
    signText_[2].setOutlineColor(sf::Color::Black);
    signText_[2].setOutlineThickness(3);

    // Load arrow textures
    leftArrow_ = resourceManager.getTexture("/assets/images/wooden_arrow2.png");
    rightArrow_ = resourceManager.getTexture("/assets/images/wooden_arrow.png");
    buttons_.resize(2);
    for (int i = 0; i < 2; ++i) {
        buttons_[i].setSize(sf::Vector2f(100, 100));
        auto globalBounds = buttons_[i].getGlobalBounds();
        buttons_[i].setOrigin(globalBounds.width / 2, globalBounds.height / 2);
        buttons_[i].setTexture(i == 0 ? &leftArrow_ : &rightArrow_);
        buttons_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -BUTTON_X_OFFSET : BUTTON_X_OFFSET), SCREEN_CENTER.y + 20);
    }

    // Load levels
    levels_.push_back({"Level 1", "level1.json", "level1.png"});
    levels_.push_back({"Level 2", "level2.json", "level2.png"});
    levels_.push_back({"Level 3", "level3.json", "level2.png"});

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
        buttons_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -BUTTON_X_OFFSET : BUTTON_X_OFFSET), SCREEN_CENTER.y + 20);
    }
    level_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);
    for (int i = 0; i < stars_.size(); i++) {
        stars_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x + 100, SCREEN_CENTER.y - 140));
    }
    for (int i = 0; i < 2; ++i) {
        sign_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -SIGN_X_OFFSET : SIGN_X_OFFSET), SCREEN_CENTER.y + SIGN_Y_OFFSET);
        signText_[i].setPosition(sign_[i].getPosition().x, sign_[i].getPosition().y);
    }
    signText_[2].setPosition(sign_[1].getPosition().x, sign_[1].getPosition().y + 20);
}

void LevelSelector::draw(sf::RenderWindow& window) const {
    for (const auto& menuItem : menuItems_) {
        window.draw(menuItem);
    }
    for (int i = 0; i < buttons_.size(); i++) {
        if (isButtonDisabled(i)) {
            continue;
        }
        window.draw(buttons_[i]);
    }
    window.draw(level_);
    for (const auto& sign : sign_) {
        window.draw(sign);
    }
    for (const auto& text : signText_) {
        window.draw(text);
    }
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
            if (isButtonDisabled(i)) {
                return Item::UNDEFINED;
            }
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
    // Player specific sign texts
    setSignTexts();
}

void LevelSelector::setSignTexts() {
    if (auto player = player_.lock()) {
        int characterSize = 70;
        int score = player->highScores.size() <= levelIndex_ ? 0 : player->highScores[levelIndex_];
        const std::vector<std::string> texts = {player->name, "High Score:", std::to_string(score)};
        // Helper lambda to set sign text
        auto setSingText = [&](int i) {
            characterSize = i == 1 ? 30 : characterSize; // Set character size for high score at least 30 at start
            signText_[i].setString(texts[i]);
            signText_[i].setCharacterSize(characterSize);
            // Find correct size for the sign text
            while (signText_[i].getGlobalBounds().width > 160 && characterSize > 10) {
                characterSize--;
                signText_[i].setCharacterSize(characterSize);
            }
            sf::FloatRect textBounds = signText_[i].getGlobalBounds();
            signText_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
            int signIndex = i == 2 ? i - 1 : i;
            int yOffset = i == 2 ? 20 : 0;
            signText_[i].setPosition(sign_[signIndex].getPosition().x, sign_[signIndex].getPosition().y + yOffset);
        };
        // Set sign text
        for (int i = 0; i < 3; ++i) {
            setSingText(i);
        }
    }
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

void LevelSelector::setLevelHighScore() {
    if (auto player = player_.lock()) {
        int score = player->highScores.size() <= levelIndex_ ? 0 : player->highScores[levelIndex_];
        signText_[2].setString(std::to_string(score));
        sf::FloatRect textBounds = signText_[2].getGlobalBounds();
        signText_[2].setOrigin(textBounds.width / 2, textBounds.height / 2);
    }
}

void LevelSelector::updateLevel() {
    setLevelStarIndex();
    setLevelHighScore();
    setLevelText();
    setLevelImage();
}

void LevelSelector::handleKeyPress(const sf::Keyboard::Key& code) {
    if (code == sf::Keyboard::Key::Left && !isButtonDisabled(0)) {
        setSelectedItem(selectedItem_ == LevelSelector::Item::PREV
            ? LevelSelector::Item::NEXT
            : LevelSelector::Item::PREV
        );
    } else if (code == sf::Keyboard::Key::Right && !isButtonDisabled(1)) {
        setSelectedItem(selectedItem_ == LevelSelector::Item::NEXT
            ? LevelSelector::Item::PREV
            : LevelSelector::Item::NEXT
        );
    } else if (code == sf::Keyboard::Key::Up) {
        setSelectedItem(selectedItem_ == LevelSelector::Item::LEVEL
            ? LevelSelector::Item::BACK
            : LevelSelector::Item::LEVEL
        );
    } else if (code == sf::Keyboard::Key::Down) {
        setSelectedItem(selectedItem_ == LevelSelector::Item::BACK
            ? LevelSelector::Item::LEVEL
            : LevelSelector::Item::BACK
        );
    }
}

bool LevelSelector::isButtonDisabled(int index) const {
    if (auto player = player_.lock()) {
        switch (index) {
            case 0:
                return levelIndex_ == 0;
            case 1:
                return levelIndex_ == levels_.size() - 1 || player->levelsCompleted <= levelIndex_;
            default:
                return false;
        } 
    }
    return true;

}
