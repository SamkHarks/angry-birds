#include "user_selector.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

UserSelector::UserSelector() {
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
        acceptText_[i].setCharacterSize(80);
        textBounds = acceptText_[i].getGlobalBounds();
        acceptText_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        acceptText_[i].setPosition(SCREEN_CENTER.x + (i == 0 ? -100 : 100), 470);
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
    playerText_.setPosition(SCREEN_CENTER.x, 470);

    // Initialize caret
    caret_.setSize(sf::Vector2f(3, playerText_.getCharacterSize()*1.3));
    caret_.setFillColor(sf::Color::White);
    caret_.setOutlineColor(sf::Color::Black);
    caret_.setOutlineThickness(5);
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
    return !player_.empty();
}

void UserSelector::setPlayer() {
    std::string playerName = playerText_.getString();
    if (playerName.length() >= 3) {
        player_ = playerName;
        setPromptText("Are you sure you want to play as " + playerName + "?", 40);
    }
}

std::string UserSelector::getPlayer() const {
    return player_;
}

void UserSelector::clearPlayer() {
    player_ = "";
    setPromptText("Add player:");
    setPlayerText("");
    isPlayerAccepted_ = false;
}

void UserSelector::setPromptText(const std::string& text, int characterSize) {
    promptText_.setString(text);
    promptText_.setCharacterSize(characterSize);
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
}

void UserSelector::setPlayerText(const std::string& text) {
    playerText_.setString(text);
    sf::FloatRect textBounds = playerText_.getGlobalBounds();
    playerText_.setOrigin(textBounds.width / 2, 0);
    playerText_.setPosition(SCREEN_CENTER.x, 470);
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