#include "user_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

UserSelector::UserSelector() : userLoader_(*this) {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    // set prompt text
    promptText_.setFont(font_);
    promptText_.setString("Add player:");
    promptText_.setFillColor(sf::Color::White);
    promptText_.setOutlineColor(sf::Color::Black);
    promptText_.setOutlineThickness(5);
    promptText_.setCharacterSize(80);
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
    promptText_.setPosition(SCREEN_CENTER.x, 400);
    // set accept text
    acceptText_.resize(2);
    for(int i = 0; i < 2; i++) {
        acceptText_[i].setFont(font_);
        acceptText_[i].setString(i == 0 ? "Yes" : "No");
        acceptText_[i].setFillColor(sf::Color::White);
        acceptText_[i].setOutlineColor(sf::Color::Black);
        acceptText_[i].setOutlineThickness(5);
        acceptText_[i].setCharacterSize(65);
        textBounds = acceptText_[i].getGlobalBounds();
        acceptText_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        acceptText_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -80 : 80), SCREEN_CENTER.y + 30);
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
    promptText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 50);
    for(int i = 0; i < 2; i++) {
        acceptText_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -80 : 80), SCREEN_CENTER.y + 30);
    }
    playerText_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y + 20);
    caret_.setPosition(
        SCREEN_CENTER.x,
        playerText_.getPosition().y
    );
} 

void UserSelector::draw(sf::RenderWindow& window) const {
    window.draw(promptText_);
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
    setPromptText("Add player:");
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
    if (isSelected) {
        acceptText_[selectedItem_].setFillColor(LIME_GREEN);
        acceptText_[selectedItem_].setScale(1.1f, 1.1f);
    } else {
        acceptText_[selectedItem_].setFillColor(sf::Color::White);
        acceptText_[selectedItem_].setScale(1.f, 1.f);
    }
}

void UserSelector::setSelectedItem(int nextItem) {
    updateItem(false);
    selectedItem_ = nextItem;
    updateItem(true);
}

int UserSelector::getItemAtPosition(sf::Vector2f mousePosition) const {
    if (isPlayerSet() && !isPlayerAccepted_) {
        for (int i = 0; i < 2; i++) {
            if (acceptText_[i].getGlobalBounds().contains(mousePosition)) {
                return i;
            }
        }
    }
    return -1;
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

void UserSelector::resetPlayer() {
    isPlayerAccepted_ = false;
    setPromptText("Continue with player: " + player_->name + "?");
}