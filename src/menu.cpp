#include "menu.hpp"
#include "utils.hpp"
#include <iostream>
#include "resource_manager.hpp"

Menu::Menu(Type type) : type_(type), menuItems_(buttonAmount_) {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    woodenSign_.setSize(sf::Vector2f(860, 860));
    woodenSign_.setTexture(&ResourceManager::getInstance().getTexture("/assets/images/wooden_sign.png"));
    woodenSign_.setOrigin(woodenSign_.getGlobalBounds().width / 2, woodenSign_.getGlobalBounds().height / 2);
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);
    backgroundImage_ = ResourceManager::getInstance().getTexture("/assets/images/background.jpg");
    backgroundMusicBuffer_ = ResourceManager::getInstance().getSoundBuffer("/assets/sounds/menu_2.wav");
    background_.setSize(sf::Vector2f(VIEW.getWidth(), VIEW.getHeight()));
    background_.setTexture(&backgroundImage_);
    background_.setPosition(0,0);
    backgroundMusic_.setBuffer(backgroundMusicBuffer_);
    backgroundMusic_.setVolume(10);
    backgroundMusic_.setLoop(true);
}

void Menu::handleResize() {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);
    background_.setSize(sf::Vector2f(VIEW.getWidth(), VIEW.getHeight()));
    background_.setPosition(0,0);
    const MenuItemLayout& layout = getMenuItemLayout();
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setPosition(VIEW.getCenter().x, (VIEW.getCenter().y - layout.yOffset) + i * layout.gap);
    }
}

const Menu::Type Menu::getType() const {
    return type_;
}


void Menu::setSelectedItem(int nextItem) {
    updateItem(sf::Color::White, 1.f);
    if (nextItem < 0) {
        selectedItem_ = buttonAmount_ - 1;
    } else if (nextItem >= buttonAmount_) {
        selectedItem_ = 0;
    } else {
        selectedItem_ = nextItem;
    }
    updateItem(LIME_GREEN, 1.1f);
}

const int Menu::getSelectedItem() const {
    return selectedItem_;
}

void Menu::updateItem(sf::Color fillColor, float scale) {
    menuItems_[selectedItem_].setFillColor(fillColor);
    menuItems_[selectedItem_].setScale(scale, scale);
}

int Menu::getItemAtPosition(sf::Vector2f mousePosition) const {
    for (int i = 0; i < buttonAmount_; i++) {
        if (menuItems_[i].getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }
    return -1;
}

bool Menu::handleMouseClick(sf::Vector2f mousePosition) {
    return getItemAtPosition(mousePosition) >= 0;
};

void Menu::handleMouseMove(sf::Vector2f mousePosition) {
    int hoveredItem = getItemAtPosition(mousePosition);
    if (hoveredItem == -1) {
        return;
    }
    updateItem(sf::Color::White, 1.f);
    selectedItem_ = hoveredItem;
    updateItem(LIME_GREEN, 1.1f);
}

void Menu::updateMusic(sf::SoundSource::Status newStatus) {
    switch (newStatus) {
        case sf::SoundSource::Status::Playing:
            backgroundMusic_.play();
            break;
        case sf::SoundSource::Status::Paused:
            backgroundMusic_.pause();
            break;
        case sf::SoundSource::Status::Stopped:
            backgroundMusic_.stop();
            break;
        default:
            break;
    }
}

void Menu::setTitle(const std::string& title, float radius, int yOffset) {
    sf::Vector2f SCREEN_CENTER = VIEW.getCenter();
    // Define the circular path segment
    sf::Vector2f center(SCREEN_CENTER.x, SCREEN_CENTER.y+yOffset);
    float startAngle = 270 - 20.f;
    float endAngle = 270 + 20.f;

    // Calculate the total width of the title
    float totalWidth = 0.0f;
    for (char c : title) {
        sf::Text tempText;
        tempText.setFont(font_);
        tempText.setString(c);
        tempText.setCharacterSize(80);
        tempText.setOutlineThickness(5);
        totalWidth += tempText.getGlobalBounds().width;
    }

    // Create title characters
    title_.resize(title.size());
    float currentAngle = startAngle;
    for (size_t i = 0; i < title.size(); ++i) {
        title_[i].setFont(font_);
        title_[i].setString(title[i]);
        title_[i].setFillColor(sf::Color::White);
        title_[i].setOutlineColor(sf::Color::Black);
        title_[i].setOutlineThickness(5);
        title_[i].setCharacterSize(80);

        // Calculate the width of the current character
        float charWidth = title_[i].getGlobalBounds().width;

        // Calculate the angle for the current character
        
        float angle = utils::DegreesToRadians(currentAngle);
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);

        // Set the position and rotation
        title_[i].setPosition(x, y);
        title_[i].setRotation(currentAngle + 90); // Adjust rotation

        // Update the current angle based on the width of the character
        currentAngle += (charWidth / totalWidth) * (endAngle - startAngle);
    }
}

const MenuItemLayout& Menu::getMenuItemLayout() const {
    if (buttonAmount_ == 1) {
        static const MenuItemLayout layout = { 80, 80, 100 };
        return layout;
    } else if (buttonAmount_ == 2) {
        static const MenuItemLayout layout = { 100, 80, 100 };
        return layout;
    } else if (buttonAmount_ == 3) {
        static const MenuItemLayout layout = { 140, 80, 100 };
        return layout;
    } else {
        static const MenuItemLayout layout = { 180, 75, 100 };
        return layout;
    }
}

void Menu::updateMenuItems() {
    const std::vector<std::string>& buttonNames = getButtonNames();
    if (buttonNames.size() == buttonAmount_ || buttonNames.empty()) {
        return; // No need to update if the amount of buttons is the same or empty
    }
    buttonAmount_ = buttonNames.size();
    menuItems_.clear();
    menuItems_.resize(buttonAmount_);
    const sf::Vector2f &SCREEN_CENTER = VIEW.getCenter();
    const MenuItemLayout &layout = getMenuItemLayout();

    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(buttonNames[i]);
        menuItems_[i].setFillColor(sf::Color::White);
        menuItems_[i].setOutlineColor(sf::Color::Black);
        menuItems_[i].setOutlineThickness(5);
        menuItems_[i].setCharacterSize(layout.characterSize);
        float width = menuItems_[i].getGlobalBounds().width;
        menuItems_[i].setOrigin(width / 2, 0);
        menuItems_[i].setPosition(SCREEN_CENTER.x , (SCREEN_CENTER.y - layout.yOffset) + i * layout.gap);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }

}
