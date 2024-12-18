#include "level_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

const float STAR_SIZE = 100;
const int SIGN_X_OFFSET = 530;
const int SIGN_Y_OFFSET = 200;
const int BUTTON_X_OFFSET = 280;
const int ITEMS = 2;

LevelSelector::LevelSelector() : buttons_(ITEMS), sign_(ITEMS), signText_(ITEMS + 1), stars_(ITEMS + 2) {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    // Load font
    std::vector<std::string> button_texts = { "Level 1", "Back" };
    for (int i = 0; i < ITEMS; ++i) {
        sf::Text text;
        text.setFont(resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf"));
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
    level_.setSize(sf::Vector2f(VIEW.getWidth(),VIEW.getHeight()));
    level_.setTexture(&resourceManager.getTexture("/assets/screenshots/level1.png"));
    level_.setOrigin(level_.getGlobalBounds().width / 2, level_.getGlobalBounds().height / 2);
    level_.setScale(0.27f, 0.27f);
    level_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);

    // Load name sign
    for (int i = 0; i < 2; ++i) {
        sign_[i].setSize(sf::Vector2f(320, 320));
        std::string path = "/assets/images/wooden_sign" + std::to_string(i + 2) + ".png";
        sign_[i].setTexture(&resourceManager.getTexture(path));
        sign_[i].setOrigin(sign_[i].getGlobalBounds().width / 2, sign_[i].getGlobalBounds().height / 2);
        int xOffset = i == 0 ? -SIGN_X_OFFSET : SIGN_X_OFFSET;
        sign_[i].setPosition(SCREEN_CENTER.x + xOffset, SCREEN_CENTER.y + SIGN_Y_OFFSET);
        signText_[i].setFont(resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf"));
        signText_[i].setFillColor(sf::Color::White);
        signText_[i].setOutlineColor(sf::Color::Black);
        signText_[i].setOutlineThickness(3);
    }
    signText_[2].setFont(resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf"));
    signText_[2].setFillColor(sf::Color::White);
    signText_[2].setOutlineColor(sf::Color::Black);
    signText_[2].setOutlineThickness(3);

    // Load arrow textures
    for (int i = 0; i < 2; ++i) {
        buttons_[i].setSize(sf::Vector2f(100, 100));
        auto globalBounds = buttons_[i].getGlobalBounds();
        buttons_[i].setOrigin(globalBounds.width / 2, globalBounds.height / 2);
        std::string path = i == 0 ? "/assets/images/wooden_arrow2.png" : "/assets/images/wooden_arrow.png";
        buttons_[i].setTexture(&resourceManager.getTexture(path));
        buttons_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -BUTTON_X_OFFSET : BUTTON_X_OFFSET), SCREEN_CENTER.y + 20);
    }

    // Load levels
    loadLevels();

    // Load stars
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
    level_.setTexture(&ResourceManager::getInstance().getTexture("/assets/screenshots/" + levels_[levelIndex_].image));
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
    if (auto player = player_.lock()) {
        return levelIndex_ < player->levelsCompleted && levelIndex_ < levels_.size() - 1;
    }
    return false;
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

// TODO: Rethink function logic later
void LevelSelector::loadLevels() {
    std::string path = utils::getExecutablePath();
    std::string levelsPath = path + "/assets/levels/";
    std::string screenShotPath = path + "/assets/screenshots/";
    std::vector<std::string> levelFiles;
    std::vector<std::string> screenShotFiles;
    int levelCount = 0;
    int screenShotCount = 0;
    // Go through the directory and find all the level files
    for (const auto& entry : fs::directory_iterator(levelsPath)) {
        if (entry.is_regular_file()) {
            auto fileName = entry.path().filename().string();
            if (fileName.find("level") != std::string::npos && fileName.find(".json") != std::string::npos) {
                levelCount++;
                levelFiles.push_back(fileName);
            }
        }
    }
    // Go through the directory and find all the screenshot files
    for (const auto& entry : fs::directory_iterator(screenShotPath)) {
        if (entry.is_regular_file()) {
            auto fileName = entry.path().filename().string();
            if (fileName.find("level") != std::string::npos && fileName.find(".png") != std::string::npos) {
                screenShotCount++;
                screenShotFiles.push_back(fileName);
            }
        }
    }

    if (levelCount != screenShotCount) {
        throw std::runtime_error("Level count does not match screenshot count");
    }

    std::sort(levelFiles.begin(), levelFiles.end());
    std::sort(screenShotFiles.begin(), screenShotFiles.end());


    for (int i = 0; i < levelCount; i++) {
        levels_.push_back({ "Level " + std::to_string(i + 1), levelFiles[i], screenShotFiles[i] });
    }

}

void LevelSelector::addNewLevel() {
    int fileCount = utils::countFilesInDirectory();
    int levelCount = levels_.size();
    if (levels_.empty()) {
        loadLevels();
    } else {
        // Add all the new levels
        while (fileCount > levelCount) {
            ++levelCount;
            Level newLevel = { "Level " + std::to_string(levelCount), "level" + std::to_string(levelCount) + ".json", "level" + std::to_string(levelCount) + ".png" };
            levels_.push_back(newLevel);
        }
    }
}
