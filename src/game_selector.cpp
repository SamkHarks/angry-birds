#include "game_selector.hpp"


GameSelector::GameSelector() : Menu(Menu::Type::GAME_SELECTOR, 0), levelSelector_(), userSelector_() {
    // Create menu items
    updateMenuItems();
    setTitle("AngryBirds", 680.f, 340);
};

void GameSelector::handleResize() {
    Menu::handleResize();
    int yOffset = 180;
    int gap = 100;
    if (buttonAmount_ == 3 || buttonAmount_ == 2) {
        yOffset = 140;
        gap = 102.5;
    }
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setPosition(VIEW.getCenter().x, (VIEW.getCenter().y - yOffset) + i * gap);
    }
    title_.clear();
    setTitle("AngryBirds", 680.f, 340);
    userSelector_.handleResize();
    levelSelector_.handleResize();
};

void GameSelector::updateMenuItems() {
    const std::vector<std::string>& button_texts = getButtonNames();
    if (button_texts.size() == buttonAmount_) {
        return; // No need to update if the amount of buttons is the same
    }
    buttonAmount_ = button_texts.size();
    menuItems_.clear();
    menuItems_.resize(buttonAmount_);
    const sf::Vector2f &SCREEN_CENTER = VIEW.getCenter();
    int yOffset = 100;
    int characterSize = 80;
    int gap = 100;

    if (buttonAmount_ == 3) {
        yOffset = 140;
    } else if (buttonAmount_ == 4) {
        yOffset = 180;
        characterSize = 75;
    }

    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::White);
        menuItems_[i].setOutlineColor(sf::Color::Black);
        menuItems_[i].setOutlineThickness(5);
        menuItems_[i].setCharacterSize(characterSize);
        float width = menuItems_[i].getGlobalBounds().width;
        menuItems_[i].setOrigin(width / 2, 0);
        menuItems_[i].setPosition(SCREEN_CENTER.x , (SCREEN_CENTER.y - yOffset) + i * gap);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }

}

void GameSelector::drawScreen(sf::RenderWindow& window) const {
    switch (screen_) {
        case GameSelector::Screen::USER_SELECTOR:
            userSelector_.draw(window);
            break;
        case GameSelector::Screen::LEVEL_SELECTOR:
            levelSelector_.draw(window);
            break;
        default:
            // Game selector screen
            for (auto menuItem : menuItems_) {
                window.draw(menuItem);
            }
            break;
    }
}

void GameSelector::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    // Draw the title characters
    for (const auto& character : title_) {
        window.draw(character);
    }
    drawScreen(window);
};

GameSelector::Screen GameSelector::getScreen() const {
    return screen_;
};

void GameSelector::setScreen(GameSelector::Screen screen) {
    screen_ = screen;
};

LevelSelector& GameSelector::getLevelSelector() {
    return levelSelector_;
};

UserSelector& GameSelector::getUserSelector() {
    return userSelector_;
};

void GameSelector::handleMouseMove(sf::Vector2f mousePosition) {
    switch (screen_) {
        case GameSelector::Screen::GAME_SELECTOR:
            this->Menu::handleMouseMove(mousePosition);
            break;
        case GameSelector::Screen::LEVEL_SELECTOR:
            levelSelector_.handleMouseMove(mousePosition);
            break;
        case GameSelector::Screen::USER_SELECTOR:
            userSelector_.handleMouseMove(mousePosition);
            break;
        default:
            break;
    }
};

bool GameSelector::handleMouseClick(sf::Vector2f mousePosition) {
    switch (screen_) {
        case GameSelector::Screen::GAME_SELECTOR:
            return this->Menu::handleMouseClick(mousePosition);
        case GameSelector::Screen::LEVEL_SELECTOR:
            return levelSelector_.handleMouseClick(mousePosition);
        case GameSelector::Screen::USER_SELECTOR:
            return userSelector_.handleMouseClick(mousePosition);
        default:
            break;
    }
    return false;
};

GameSelector::Item GameSelector::getSelectedItem() const {
    int itemCount = menuItems_.size();
    if (screen_ == GameSelector::Screen::GAME_SELECTOR && itemCount > 0 && selectedItem_ < itemCount) {
       auto item = menuItems_[selectedItem_].getString();
       if (item == "New Game") {
           return Item::NEW_GAME;
       } else if (item == "Load Game") {
           return Item::LOAD_GAME;
       } else if (item == "Continue") {
           return Item::CONTINUE;
       } else if (item == "Back") {
           return Item::BACK;
       } else {
           return Item::UNDEFINED;
       }
    } else {
        return Item::UNDEFINED;
    }
};

const std::vector<std::string>& GameSelector::getButtonNames() const {
    int userSize = userSelector_.getPlayers().size();
    bool isPlayerSet = userSelector_.isPlayerSet();
    if (userSize > 0 && !isPlayerSet) {
        static const std::vector<std::string> button_texts = { "New Game", "Load Game", "Back" };
        return button_texts;
    } else if (userSize > 1 && isPlayerSet) {
        static const std::vector<std::string> button_texts = { "Continue", "New Game", "Load Game", "Back" };
        return button_texts;
    } else if (userSize == 1 && isPlayerSet) {
        static const std::vector<std::string> button_texts = {  "Continue", "New Game", "Back" };
        return button_texts;
    } else {
    static const std::vector<std::string> button_texts = { "New Game", "Back" };
    return button_texts;
    }
};