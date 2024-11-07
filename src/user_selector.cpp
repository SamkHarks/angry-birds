#include "user_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

UserSelector::UserSelector() : userLoader_(*this) {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    // set prompt text
    promptText_.setFont(font_);
    promptText_.setString("Add new player:");
    promptText_.setFillColor(sf::Color::White);
    promptText_.setOutlineColor(sf::Color::Black);
    promptText_.setOutlineThickness(5);
    promptText_.setCharacterSize(80);
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
    promptText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 50);
    // set accept text
    std::vector<std::string> texts = { "Yes", "No", "Back" };
    acceptText_.resize(texts.size());
    for(int i = 0; i < texts.size(); i++) {
        acceptText_[i].setFont(font_);
        acceptText_[i].setString(texts[i]);
        acceptText_[i].setFillColor(sf::Color::White);
        acceptText_[i].setOutlineColor(sf::Color::Black);
        acceptText_[i].setOutlineThickness(5);
        acceptText_[i].setCharacterSize(65);
        textBounds = acceptText_[i].getGlobalBounds();
        acceptText_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        int xOffset = i == 0 ? -80 : i == 1 ? 80 : 0;
        int yOffset = i > 1 ? 170 : 30;
        acceptText_[i].setPosition(SCREEN_CENTER.x + (xOffset), SCREEN_CENTER.y + yOffset);
    }
    // set player text
    playerText_.setFont(font_);
    playerText_.setFillColor(sf::Color::White);
    playerText_.setOutlineColor(sf::Color::Black);
    playerText_.setOutlineThickness(5);
    playerText_.setCharacterSize(70);
    playerText_.setString("");
    textBounds = playerText_.getGlobalBounds();
    playerText_.setOrigin(textBounds.width / 2, 0);
    playerText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);

    // Initialize caret
    caret_.setSize(sf::Vector2f(3, playerText_.getCharacterSize()*1.3));
    caret_.setFillColor(sf::Color::White);
    caret_.setOutlineColor(sf::Color::Black);
    caret_.setOutlineThickness(5);
    caret_.setPosition(
        SCREEN_CENTER.x,
        playerText_.getPosition().y
    );
    // Set initial selected item to Yes
    selectedItem_ = 0;
    updateItem(true);

    // load players
    userLoader_.loadPlayers();
}

void UserSelector::handleResize() {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    sf::Vector2f propmptPosition = screen_ == UserSelector::Screen::NEW_PLAYER
        ? sf::Vector2f({SCREEN_CENTER.x, SCREEN_CENTER.y - 50})
        : sf::Vector2f({SCREEN_CENTER.x + 10, SCREEN_CENTER.y - 95});
    promptText_.setPosition(propmptPosition);
    for(int i = 0; i < 3; i++) {
        int xOffset = i == 0 ? -80 : i == 1 ? 80 : 0;
        int yOffset = i > 1 ? 170 : 30;
        acceptText_[i].setPosition(SCREEN_CENTER.x + (xOffset), SCREEN_CENTER.y + yOffset);
    }
    playerText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);
    caret_.setPosition(
        SCREEN_CENTER.x,
        playerText_.getPosition().y
    );
    for (int i = 0; i < playerNames_.size(); i++) {
        playerNames_[i].setPosition(SCREEN_CENTER.x - 200, (SCREEN_CENTER.y - 50) + i * 50);
    }
}

void UserSelector::initializePlayerNames() {
    const int userCount = userLoader_.getPlayers().size();
    if (userCount == playerNames_.size()) {
        return; // No need to reinitialize
    }
    const sf::Vector2f& SCREEN_CENTER = VIEW.getCenter();
    // Initialize player names
    playerNames_.clear();
    playerNames_.resize(userCount);
    for (int i = 0; i < userCount; i++) {
        playerNames_[i].setFont(font_);
        playerNames_[i].setFillColor(sf::Color::White);
        playerNames_[i].setOutlineColor(sf::Color::Black);
        playerNames_[i].setOutlineThickness(5);
        playerNames_[i].setCharacterSize(50);
        playerNames_[i].setString(userLoader_.getPlayers()[i].name);
        sf::FloatRect textBounds = playerNames_[i].getGlobalBounds();
        playerNames_[i].setPosition(SCREEN_CENTER.x - 200, (SCREEN_CENTER.y - 50) + i * 50);
    }
}

void UserSelector::drawNewPlayer(sf::RenderWindow& window) const {
    if (isPlayerSet() && !isPlayerAccepted_) {
        window.draw(acceptText_[0]);
        window.draw(acceptText_[1]);
    } else {
        window.draw(playerText_);
        if (((int) caretClock_.getElapsedTime().asSeconds() % 2) == 1) {
            window.draw(caret_);
        }
    }
}

void UserSelector::drawLoadPlayer(sf::RenderWindow& window) const {
    for (const auto& playerName : playerNames_) {
        window.draw(playerName);
    }
    window.draw(acceptText_[2]);
}

void UserSelector::draw(sf::RenderWindow& window) const {
    window.draw(promptText_);
    if (screen_ == UserSelector::Screen::NEW_PLAYER) {
        drawNewPlayer(window);
    } else {
        drawLoadPlayer(window);
    }
}

bool UserSelector::isPlayerSet() const {
    return player_ != nullptr && !player_->name.empty();
}

void UserSelector::setPlayer() {
    std::string playerName = playerText_.getString();
    if (playerName.length() >= 3) {
        if (userLoader_.isPlayerNameAvailable(playerName)) {
            player_ = std::make_shared<Player>(Player{playerName, {}, {}, 0});
            setPromptText("Create new player: " + playerName + "?");
        } else {
            userLoader_.loadPlayer(playerName);
            setPromptText("Player: " + playerName + " already exists. Load player?");
        }
        
    }
}

const std::shared_ptr<Player>& UserSelector::getPlayer() const {
    return player_;
}

void UserSelector::clearPlayer() {
    player_.reset();
    setPromptText("Add new player:");
    setPlayerText("");
    isPlayerAccepted_ = false;
}

void UserSelector::setPromptText(const std::string& text) {
    int characterSize = 80;
    promptText_.setString(text);
    promptText_.setCharacterSize(characterSize);
    while (promptText_.getGlobalBounds().width > 685 && characterSize > 10) {
        characterSize--;
        promptText_.setCharacterSize(characterSize);
    }
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
}

void UserSelector::setPlayerText(const std::string& text) {
    const sf::Vector2f& SCREEN_CENTER = VIEW.getCenter();
    playerText_.setString(text);
    sf::FloatRect textBounds = playerText_.getGlobalBounds();
    playerText_.setOrigin(textBounds.width / 2, 0);
    playerText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);
    caret_.setPosition(
        playerText_.getPosition().x + textBounds.width / 2,
        playerText_.getPosition().y
    );
}

std::string UserSelector::getPlayerText() const {
    return playerText_.getString();
}

void UserSelector::setPlayerAccepted(bool accepted) {
    isPlayerAccepted_ = accepted;
}

bool UserSelector::isPlayerAccepted() const {
    return isPlayerAccepted_;
}

void UserSelector::updateItem(bool isSelected) {
    const UserSelector::Item item = convertIndexToItem();
    switch (item) {
        case UserSelector::Item::ACCEPT:
        case UserSelector::Item::CANCEL:
        case UserSelector::Item::BACK:
            if (isSelected) {
                acceptText_[selectedItem_].setFillColor(LIME_GREEN);
                acceptText_[selectedItem_].setScale(1.1f, 1.1f);
            } else {
                acceptText_[selectedItem_].setFillColor(sf::Color::White);
                acceptText_[selectedItem_].setScale(1.f, 1.f);
            }
            break;
        case UserSelector::Item::PLAYER_NAME:
            isSelected
                ? playerNames_[getSelectedPlayerIndex()].setFillColor(LIME_GREEN)
                : playerNames_[getSelectedPlayerIndex()].setFillColor(sf::Color::White);
            break;
    }

}

void UserSelector::setSelectedItem(int nextItem) {
    updateItem(false);
    selectedItem_ = nextItem;
    updateItem(true);
}

int UserSelector::getItemAtPosition(sf::Vector2f mousePosition) const {
    if (screen_ == UserSelector::Screen::NEW_PLAYER) {
        if (isPlayerSet() && !isPlayerAccepted_) {
            for (int i = 0; i < 2; i++) {
                if (acceptText_[i].getGlobalBounds().contains(mousePosition)) {
                    return i;
                }
            }
        }
    } else if (screen_ == UserSelector::Screen::LOAD_PLAYER) {
        if (acceptText_[2].getGlobalBounds().contains(mousePosition)) {
            return 2;
        }
        // Check player names and return convereted index if found 
        for (int i = 0; i < playerNames_.size(); i++) {
            if (playerNames_[i].getGlobalBounds().contains(mousePosition)) {
                return i + 3;
            }
        }
    }
    return -1;
}

const UserSelector::Item UserSelector::convertIndexToItem() const {
    if (screen_ == UserSelector::Screen::NEW_PLAYER) {
        if (selectedItem_ >= 0 && selectedItem_ < 2) {
            return static_cast<Item>(selectedItem_);
        }
    } else if (screen_ == UserSelector::Screen::LOAD_PLAYER) {
        if (selectedItem_ == 2) {
            return Item::BACK;
        }
        if (selectedItem_ >= 3 && selectedItem_ < playerNames_.size() + 3) {
            return Item::PLAYER_NAME;
        }
    }
    return Item::UNDEFINED;
}

const int UserSelector::getSelectedPlayerIndex() const {
    return selectedItem_ - 3;
}

const int UserSelector::getSelectedItem() const {
    return selectedItem_;
}

bool UserSelector::handleMouseClick(sf::Vector2f mousePosition) {
    return getItemAtPosition(mousePosition) >= 0;
}

void UserSelector::handleMouseMove(sf::Vector2f mousePosition) {
    int hoveredItem = getItemAtPosition(mousePosition);
    if (hoveredItem == -1) {
        return;
    }
    updateItem(false);
    selectedItem_ = hoveredItem;
    updateItem(true);
}

UserSelector::Screen UserSelector::getScreen() const {
    return screen_;
}

void UserSelector::setScreen(UserSelector::Screen screen) {
    screen_ = screen;
}

bool UserSelector::isNewPlayer() const {
    return userLoader_.isPlayerNameAvailable(player_->name);
}

void UserSelector::savePlayer() {
    userLoader_.savePlayer();
}

void UserSelector::loadPlayer() {
    assert (selectedItem_ >= 3 && selectedItem_ < userLoader_.getPlayers().size() + 3);
    userLoader_.loadPlayer(getSelectedPlayerIndex());
}

const std::vector<Player>& UserSelector::getPlayers() const {
    return userLoader_.getPlayers();
}

void UserSelector::initializeScreen() {
    const sf::Vector2f& SCREEN_CENTER = VIEW.getCenter();
    selectedItem_ = 0;
    switch (screen_) {
        case Screen::NEW_PLAYER:
            clearPlayer();
            promptText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 50);
            break;
        case Screen::LOAD_PLAYER:
            initializePlayerNames();
            setPromptText("Select player:");
            promptText_.setPosition(SCREEN_CENTER.x + 10, SCREEN_CENTER.y - 95);
            break;
    }
}