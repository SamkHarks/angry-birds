#include "pause.hpp"
#include "utils.hpp"
#include <iostream>

Pause::Pause() : Menu(Menu::Type::PAUSE) {
    // Create menu items
    updateMenuItems();
}

void Pause::handleResize() {
}

void Pause::handleResize(const sf::RenderWindow& window) {
    updatePosition(window);
}

void Pause::draw(sf::RenderWindow& window) const {
    window.draw(woodenSign_);
    // Draw the title characters
    for (const auto& character : title_) {
        window.draw(character);
    }
    // Draw menu items
    for (auto menuItem : menuItems_) {
        window.draw(menuItem);
    }
}

void Pause::updatePosition(const sf::RenderWindow& window) {
    const sf::Vector2f& SCREEN_CENTER = VIEW.getCenter();
    woodenSign_.setPosition(window.mapPixelToCoords(sf::Vector2i(SCREEN_CENTER.x, SCREEN_CENTER.y - 30)));
    auto& layout = getMenuItemLayout();
    for (size_t i = 0; i < menuItems_.size(); ++i) {
        menuItems_[i].setPosition(window.mapPixelToCoords(sf::Vector2i(SCREEN_CENTER.x, SCREEN_CENTER.y - layout.yOffset + i * layout.gap)));
    }
    updateTitle(window);
}

void Pause::updateTitle(const sf::RenderWindow& window) {
    int yOffset = 340;
    float radius = 680;
    std::string title = "Game Paused";
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
    title_.clear();
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
        title_[i].setPosition(window.mapPixelToCoords(sf::Vector2i(x, y)));
        title_[i].setRotation(currentAngle + 90); // Adjust rotation

        // Update the current angle based on the width of the character
        currentAngle += (charWidth / totalWidth) * (endAngle - startAngle);
    }
}

const std::vector<std::string>& Pause::getButtonNames() const {
    static const std::vector<std::string> buttonNames = { "Resume", "Restart", "Main Menu", "Exit" };
    return buttonNames;
}