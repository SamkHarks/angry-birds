#include "game_selector.hpp"


GameSelector::GameSelector() : Menu(Menu::Type::GAME_SELECTOR), levelSelector_(), userSelector_() {
    // Create menu items
    updateMenuItems();
    setTitle("AngryBirds", 680.f, 340);
};

void GameSelector::handleResize() {
    Menu::handleResize();
    title_.clear();
    setTitle("AngryBirds", 680.f, 340);
    userSelector_.handleResize();
    levelSelector_.handleResize();
};

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
}
